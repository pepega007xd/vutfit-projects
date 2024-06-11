# Program 1

When running the program under `strace`, we see that the program first opens a file `output.txt`, and then repeatedly writes into it, always using `lseek` to begin writing the next bytes from the start of the file:

```
openat(AT_FDCWD, "output.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644) = 3
lseek(3, 0, SEEK_SET)                   = 0
write(3, "This is a 64-byte string to be w"..., 51) = 51
lseek(3, 0, SEEK_SET)                   = 0
write(3, "This is a 64-byte string to be w"..., 51) = 51
```

When profiling the program using `perf record` and displaying the profile with `perf report`, we see most overhead in libc or kernel functions related to these IO operations, caused by these writes:

```
Samples: 23K of event 'cycles', Event count (approx.): 14281362823
Overhead  Command    Shared Object         Symbol
   6.97%  program-1  [kernel.vmlinux]      [k] entry_SYSCALL_64         
   5.67%  program-1  [kernel.vmlinux]      [k] exit_to_user_mode_prepare
   4.48%  program-1  [kernel.vmlinux]      [k] __filemap_get_folio      
   4.22%  program-1  [kernel.vmlinux]      [k] iomap_apply              
   3.36%  program-1  [kernel.vmlinux]      [k] __fget_light             
   2.86%  program-1  libc.so.6             [.] __write                  
   2.67%  program-1  [kernel.vmlinux]      [k] down_write               
   2.63%  program-1  libc.so.6             [.] __lseek   
```

When counting used syscalls using `perf stat -e 'syscalls:sys_enter_*' ./program-1`, we see that the program made almost a million writes and seeks in the three seconds of runtime:

```
           930,062      syscalls:sys_enter_write                                     
         
                 1      syscalls:sys_enter_read                                      
         
           930,062      syscalls:sys_enter_lseek                                     

        ...

       2.827705753 seconds time elapsed

       0.105855000 seconds user
       2.705844000 seconds sys
```

# Program 2

When running `perf stat ./program-2`, we see that the time spent in the kernel is already very short:

```
Loop time: 2.962609 seconds

               295      context-switches                 #   99.329 /sec             

        ...

       2.962750000 seconds user
       0.000998000 seconds sys
```

From what I can read from the profile acquired by running `perf record ./program-2`, the only kernel overhead comes from interrupts caused by the APIC timer (`asm_sysvec_apic_timer_interrupt`) which are used by the scheduler, but the overhead is negligible (around 0.01%).

```
  99.79%  program-2  program-2             [.] fibonacci                       
   0.02%  program-2  [kernel.kallsyms]     [k] __hrtimer_run_queues           
   0.02%  program-2  [kernel.kallsyms]     [k] restore_fpregs_from_fpstate    
   0.01%  program-2  [kernel.kallsyms]     [k] put_prev_entity                
   0.01%  program-2  [kernel.kallsyms]     [k] __update_load_avg_cfs_rq       
   0.01%  program-2  [kernel.kallsyms]     [k] native_sched_clock             
   0.01%  program-2  [kernel.kallsyms]     [k] asm_sysvec_apic_timer_interrupt
   0.01%  program-2  [kernel.kallsyms]     [k] __get_user_8                   
   0.01%  program-2  [kernel.kallsyms]     [k] psi_task_switch                
   0.01%  program-2  [kernel.kallsyms]     [k] __sysvec_apic_timer_interrupt
```

# Program 3

When running the single-threaded version under `perf trace`, we see that the program does not manipulate the scheduling policy of its own process in any way:

```
$ perf trace ./program-3-single

     ...
     0.891 ( 0.011 ms): program-3-sing/54503 mprotect(start: 0x7f8ac123f000, len: 8192, prot: READ)                = 0
     0.937 ( 0.004 ms): program-3-sing/54503 prlimit64(resource: STACK, old_rlim: 0x7fff978fdc10)                  = 0
     0.964 ( 0.019 ms): program-3-sing/54503 munmap(addr: 0x7f8ac1201000, len: 29543)                              = 0
     1.029 ( 0.008 ms): program-3-sing/54503 clock_gettime(which_clock: PROCESS_CPUTIME_ID, tp: 0x7fff978fde40)    = 0

     ... // a single loop
     3323.360 ( 0.007 ms): program-3-sing/54503 clock_gettime(which_clock: PROCESS_CPUTIME_ID, tp: 0x7fff978fde40)    = 0
     3323.373 ( 0.008 ms): program-3-sing/54503 write(fd: 1</dev/pts/0>, buf: 0x23db2a0, count: 28)                   = 28
     3323.382 ( 0.001 ms): program-3-sing/54503 clock_gettime(which_clock: PROCESS_CPUTIME_ID, tp: 0x7fff978fde40)    = 0
Loop time: 1.165057 seconds
```

However, when running the multithreaded version, we see that the parent thread (pid 54519) first sets its own scheduler to the realtime scheduling algorithm FIFO (`sched_setscheduler`), and sets its priority to 50 (`sched_setaffinity`, range 1 - 99), and then launches multiple child processes (pid 54521 shown selectively as an example). In each process, the scheduling algorithm is changed again to "Round Robin" (`sched_setscheduler(policy: RR, ...)`), and then the loop is executed.

```
$ perf trace ./program-3-multi

     ...
     1.037 ( 0.017 ms): program-3-mult/54519 sched_setaffinity(len: 128, user_mask_ptr: 0x7ffdb7b77a70)            = 0
     1.059 ( 0.004 ms): program-3-mult/54519 sched_getparam(param: 0x7ffdb7b77a6c)                                 = 0
     1.069 ( 0.035 ms): program-3-mult/54519 sched_setscheduler(policy: FIFO, param: 0x7ffdb7b77a6c)               = 0


     ... // a single loop
     1001.576 ( 0.370 ms): program-3-mult/54519 clone(clone_flags: CHILD_CLEARTID|CHILD_SETTID|0x11, child_tidptr: 0x7fbe4091ca10) = 54521 (program-3-multi)
     1002.022 ( 0.004 ms): program-3-mult/54521 set_robust_list(head: 0x7fbe4091ca20, len: 24)                        = 0
     1002.145 ( 0.008 ms): program-3-mult/54521 clock_gettime(which_clock: PROCESS_CPUTIME_ID, tp: 0x7ffdb7b77a30)    = 0
     1002.166 ( 0.013 ms): program-3-mult/54521 sched_setscheduler(policy: RR, param: 0x7ffdb7b77a6c)                 = 0
     ...
     8787.542 ( 0.016 ms): program-3-mult/54521 clock_gettime(which_clock: PROCESS_CPUTIME_ID, tp: 0x7ffdb7b77a30)    = 0
     8787.625 ( 0.027 ms): program-3-mult/54521 newfstatat(dfd: 1, filename: 0x40ad6f35, statbuf: 0x7ffdb7b772f0, flag: 4096) = 0
     8787.666 ( 0.004 ms): program-3-mult/54521 getrandom(buf: 0x7fbe40b204d8, count: 8, flags: NONBLOCK)             = 8
     8787.683 ( 0.003 ms): program-3-mult/54521 brk()                                                                 = 0x95a000
     8787.693 ( 0.012 ms): program-3-mult/54521 brk(brk: 0x97b000)                                                    = 0x97b000
     8787.729 ( 0.017 ms): program-3-mult/54521 write(fd: 1, buf: 0x95a2a0, count: 28)                                = 28
     8787.761 (         ): program-3-mult/54521 exit_group()                                                          = ?
     6004.038 (2783.856 ms): program-3-mult/54519  ... [continued]: wait4())                                            = 54521 (program-3-multi)
```

When I remove the changes done by all the processes using a dummy implementation compiled using `gcc preload.c -o preload.so -fPIC -shared -ldl`, and loaded using `LD_PRELOAD='./preload.so' ./program-3-multi`, the loop time improves significantly:

preload.c:
```
int sched_setscheduler() {
	return 0;
}
```

```
$ ./program-3-single 
Loop time: 1.886267 seconds
Loop time: 1.169207 seconds
Loop time: 1.168635 seconds
Loop time: 1.168548 seconds

$ ./program-3-multi 
Loop time: 1.948685 seconds
Loop time: 1.946673 seconds
Loop time: 1.964487 seconds
Loop time: 1.986989 seconds

$ LD_PRELOAD='./preload.so' ./program-3-multi # with scheduler changes removed
Loop time: 1.753318 seconds
Loop time: 1.362837 seconds
Loop time: 1.191904 seconds
Loop time: 1.191547 seconds
```

# Program 4

```
$ date
Wed  8 May 23:56:49 CEST 2024
```

Dosel cas :(
