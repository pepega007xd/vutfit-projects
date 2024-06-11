When opening, `crash` tells us that the kernel panicked on "hung_task":

```
$ crash vmlinux vmcore_p3
    ...
    PANIC: "Kernel panic - not syncing: hung_task: blocked tasks"
    ...
```

This means that a kernel process, or a userspace process invoking the kernel via syscall was running for too long without returning to userspace, and the kernel detected this as a bug and panicked.

```
crash> log
[   51.555322] smajdalf: loading out-of-tree module taints kernel.
[   51.558281] Smajdalf: Carodej nikdy nechodi pozde.
...
[  245.808706] INFO: task systemd:1 blocked for more than 120 seconds.
```

From the kernel log, we can see that a module `smajdalf` was loaded, and some time after that, the kernel detected that it is blocked. This happened when running the `systemd` userspace process, which I believe is not important (any process could have been scheduled at the moment of detection).

```
crash> bt
PID: 42       TASK: ff47e40b7fac5ac0  CPU: 2    COMMAND: "khungtaskd"
 #0 [ff7ea329804a3d28] machine_kexec at ffffffff89e635ce
 #1 [ff7ea329804a3d80] __crash_kexec at ffffffff89f9d6bd
 #2 [ff7ea329804a3e48] panic at ffffffff89eeb227
 #3 [ff7ea329804a3ec8] watchdog at ffffffff89fd355f
 #4 [ff7ea329804a3f10] kthread at ffffffff89f0e906
 #5 [ff7ea329804a3f50] ret_from_fork at ffffffff8a80023f
```

We get a backtrace of the kernel process `khungtaskd`, which gets scheduled only every 120 seconds, and checks for hung processes. We can see the `watchdog` function, which actually does the checking, and which called the `panic` function. We can look into the source code by printing the code above the return address `ffffffff89fd355f`:

```
crash> dis -lr ffffffff89fd355f 3
/usr/src/debug/kernel-4.18.0-348.12.2.el8_5/linux-4.18.0-348.12.2.el8_5.x86_64/kernel/hung_task.c: 198
0xffffffff89fd3553 <watchdog+547>:      mov    $0xffffffff8aedc669,%rdi
0xffffffff89fd355a <watchdog+554>:      call   0xffffffff89eeb130 <panic>
/usr/src/debug/kernel-4.18.0-348.12.2.el8_5/linux-4.18.0-348.12.2.el8_5.x86_64/./include/linux/sched/task.h: 102
0xffffffff89fd355f <watchdog+559>:      mov    %rbp,%rdi
```

kernel/hung_task.c: 198
```
/*
 * Check whether a TASK_UNINTERRUPTIBLE does not get woken up for
 * a really long time (120 seconds). If that happens, print out
 * a warning.
 */
static void check_hung_uninterruptible_tasks(unsigned long timeout)
{
    ...
		panic("hung_task: blocked tasks"); <---------- 198
```

This is the call that caused the kernel panic. From the function's code, we can see that if `hung_task_call_panic` was set to `false` (this is configurable), the panic would not have happened. This is one way to prevent a crash like this.

However, the function simply measures time, it does not tell us *why* the was the kernel blocked. Let's examine the module itself.

```
crash> sym -m smajdalf
ffffffffc0922000 MODULE START: smajdalf
ffffffffc0922000 (t) take_the_lock_of_power
ffffffffc0922060 (t) trik_se_spicatym_kloboukem
ffffffffc092207d (t) smajdalf_cleanup
ffffffffc092207d (t) cleanup_module
ffffffffc0924000 (d) smajdalf_dir_table
ffffffffc0924080 (d) smajdalf_table
ffffffffc0924100 (d) magic_mutex
ffffffffc0924140 (d) __this_module
ffffffffc09244c0 (b) smajdalf_sysctl_header
ffffffffc0926000 MODULE END: smajdalf
```

Fortunately, the module contains symbols. We can see that the module contains some kind of sysctl header. Sysctl is a form of interface for userspace processes to communicate with the kernel. Kernel module can register a handler, and the kernel will create a character device in `/proc/sys/...` corresponding to that handler. The user can then simply `echo "something" > /proc/sys/my_handler` and the handler in the kernel module will be executed.

When loading the module, the kernel registers the sysctl handler according to a `ctl_table` struct inside the module. The structure is recursive, so that the module can create a hierarchy of handlers in `/proc/sys`. We can look at these structs:

```
crash> struct ctl_table smajdalf_dir_table
struct ctl_table {
  procname = 0xffffffffc09230ab <cleanup_module+4142> "spellbook",
  data = 0x0,
  maxlen = 0,
  mode = 365,
  child = 0xffffffffc0924080 <smajdalf_table>,
  proc_handler = 0x0,
  poll = 0x0,
  extra1 = 0x0,
  extra2 = 0x0
}

crash> struct ctl_table smajdalf_table
struct ctl_table {
  procname = 0xffffffffc09230b5 <cleanup_module+4152> "trik_se_spicatym_kloboukem",
  data = 0x0,
  maxlen = 0,
  mode = 128,
  child = 0x0,
  proc_handler = 0xffffffffc0922000 <take_the_lock_of_power>,
  poll = 0x0,
  extra1 = 0x0,
  extra2 = 0x0
}
```

The first table corresponds to a directory (it has no `proc_handler`) in `/sys/ctl` named `spellbook`, and its child is `smajdalf_table`, which has the handler `take_the_lock_of_power`.

```
crash> dis take_the_lock_of_power
0xffffffffc0922000 <take_the_lock_of_power>:    nopl   0x0(%rax,%rax,1) [FTRACE NOP]
0xffffffffc0922005 <take_the_lock_of_power+5>:  mov    -0x35508fcc(%rip),%rax        # 0xffffffff8b419040 <init_task+2048>
0xffffffffc092200c <take_the_lock_of_power+12>: push   %rbx
0xffffffffc092200d <take_the_lock_of_power+13>: cmp    $0xffffffff8b419040,%rax
0xffffffffc0922013 <take_the_lock_of_power+19>: je     0xffffffffc092201e <take_the_lock_of_power+30>
0xffffffffc0922015 <take_the_lock_of_power+21>: cmpl   $0x1,0x100(%rax)
0xffffffffc092201c <take_the_lock_of_power+28>: je     0xffffffffc092204c <take_the_lock_of_power+76>
0xffffffffc092201e <take_the_lock_of_power+30>: mov    $0x9502f8ff,%ebx
0xffffffffc0922023 <take_the_lock_of_power+35>: pause  
0xffffffffc0922025 <take_the_lock_of_power+37>: call   0xffffffff8a77a850 <_cond_resched>
0xffffffffc092202a <take_the_lock_of_power+42>: sub    $0x1,%rbx
0xffffffffc092202e <take_the_lock_of_power+46>: jne    0xffffffffc0922023 <take_the_lock_of_power+35>
0xffffffffc0922030 <take_the_lock_of_power+48>: mov    $0xffffffffc0924100,%rdi
0xffffffffc0922037 <take_the_lock_of_power+55>: call   0xffffffff8a77c9c0 <mutex_lock>
0xffffffffc092203c <take_the_lock_of_power+60>: mov    $0xffffffffc0924100,%rdi
0xffffffffc0922043 <take_the_lock_of_power+67>: call   0xffffffff8a77c9c0 <mutex_lock>
0xffffffffc0922048 <take_the_lock_of_power+72>: xor    %eax,%eax
0xffffffffc092204a <take_the_lock_of_power+74>: pop    %rbx
0xffffffffc092204b <take_the_lock_of_power+75>: ret    
0xffffffffc092204c <take_the_lock_of_power+76>: mov    0x50(%rax),%rdi
0xffffffffc0922050 <take_the_lock_of_power+80>: add    $0x70,%rdi
0xffffffffc0922054 <take_the_lock_of_power+84>: call   0xffffffff8a77cf30 <down_write>
0xffffffffc0922059 <take_the_lock_of_power+89>: jmp    0xffffffffc092201e <take_the_lock_of_power+30>
```

This function first loads a value from address `0xffffffff8b419040` into `rax`. This value corresponds to the `next` field on a list of children, and the program checks it against the pointer value itself. In the kernel, lists are cyclic, so this would mean that for the `je` jump to execute, the `init_task` (first process) would have to be the only task running. Then the process's pid (offset 2148) is compared to 1. This would skip to the end of function. But we can get the pid of the init process:

```
crash> struct task_struct init_task | grep pid
  pid = 0,
```

...and it is zero, so the function continues. It loads the value 0x9502f8ff into `ebx` and loops until it's decremented to zero, always rescheduling to another process (`_cond_resched`).

Then the function loads an address 0xffffffffc0924100 into `rdi` (first function argument), and calls `mutex_lock`. It then does this again. If no other process unlocks this mutex, this would mean a deadlock.

```
crash> kmem 0xffffffffc0924100
ffffffffc0924100 (d) magic_mutex [smajdalf]
...

crash> struct -x mutex magic_mutex
struct mutex {
  owner = {
    counter = 0xff47e40b06683c81
  },
  wait_lock = {
    {
      rlock = {
        raw_lock = {
          {
            val = {
              counter = 0x0
            },
            {
              locked = 0x0,
              pending = 0x0
            },
            {
              locked_pending = 0x0,
              tail = 0x0
            }
          }
        }
      }
    }
  },
  osq = {
    tail = {
      counter = 0x0
    }
  },
  wait_list = {
    next = 0xff7ea32980ae7e00,
    prev = 0xff7ea32980ae7e00
  }
}
```

We can see that the mutex's address is referenced by the symbol `magic_mutex` (a symbol from our module `smajdalf`), so it cannot be referenced (and unlocked) by any code inside the kernel. When looking through the structure `mutex` and reading through [the documentation](https://docs.kernel.org/locking/mutex-design.html), it is clear that the `counter` field inside the `owner` struct refers to the task_struct of the task holding the mutex. NULL value would mean that the mutex is unlocked, which is clearly not the case. The mutex internally uses a spinlock to make accesses to it atomic, but this spinlock is unlocked, so at the time of crash, no other process was trying to acquire the lock. (This shouldn't be true, because the `<take_the_lock_of_power` tries to take the mutex twice, but I think that the watchdog or khungtaskd detects this and clears the queue - it should be able to recover the kernel from a deadlock, if the kernel wasn't configured to panic in this case).

```
crash> kmem 0xff47e40b06683c81
CACHE             OBJSIZE  ALLOCATED     TOTAL  SLABS  SSIZE  NAME
ff47e40b0101d6c0     7688        196       228     57    32k  task_struct
  SLAB              MEMORY            NODE  TOTAL  ALLOCATED  FREE
  fffadfbe4019a000  ff47e40b06680000     0      4          4     0
  FREE / [ALLOCATED]
  [ff47e40b06683c80]

    PID: 1706
COMMAND: "bash"
   TASK: ff47e40b06683c80  [THREAD_INFO: ff47e40b06683c80]
    CPU: 0
  STATE: TASK_UNINTERRUPTIBLE 
```

We can see that the struct refers to a bash process, and it is in uninterruptible state (during a syscall). 

From this, we can reconstruct the timeline:

- The user loaded the module `smajdalf`
- This module registered a sysctl handler at `/proc/sys/spellbook/trik_se_spicatym_kloboukem`
- User wrote some data into this handler
- The handler first counted in a loop for a while
- Then it deadlocked itself
- khungtaskd detected this after some time and panicked

Possible solutions to this:
- configure kernel not to panic on hung_task
- make the init process have the pid 1 (the deadlock code would be skipped in this case)
- DO NOT let smajdalf perform "trik se spicatym kloboukem"
