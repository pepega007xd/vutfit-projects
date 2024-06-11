After starting `crash`, we can see that the kernel crashed because of an OOM panic:

```
$ crash vmlinux vmcore_p2
    ...
    MEMORY: 2 GB
    PANIC: "Kernel panic - not syncing: Out of memory: system-wide panic_on_oom is enabled"
    PID: 2359
    COMMAND: "stress"
    ...
```

At the time of the crash, `stress` was the running process on the core, on which the panic happened.
We can also see that `panic_on_oom` was set, instructing the kernel not to kill any programs at the point of OOM, but to cause a panic.
Let's see how exactly the crash happened:

```
crash> bt
PID: 2359     TASK: ff38963f46e65ac0  CPU: 0    COMMAND: "stress"
 #0 [ff41a7a440acfa38] machine_kexec at ffffffff9e6635ce
 #1 [ff41a7a440acfa90] __crash_kexec at ffffffff9e79d6bd
 #2 [ff41a7a440acfb58] panic at ffffffff9e6eb227
 #3 [ff41a7a440acfbd8] out_of_memory.cold.35 at ffffffff9e87e6f1
 #4 [ff41a7a440acfc18] __alloc_pages_slowpath at ffffffff9e8d4825
 #5 [ff41a7a440acfd10] __alloc_pages_nodemask at ffffffff9e8d4beb
 #6 [ff41a7a440acfd70] alloc_pages_vma at ffffffff9e8ef414
 #7 [ff41a7a440acfdb0] do_anonymous_page at ffffffff9e8b1077
 #8 [ff41a7a440acfde8] __handle_mm_fault at ffffffff9e8b7336
 #9 [ff41a7a440acfe98] handle_mm_fault at ffffffff9e8b742e
#10 [ff41a7a440acfec0] __do_page_fault at ffffffff9e674f5d
#11 [ff41a7a440acff20] do_page_fault at ffffffff9e675267
#12 [ff41a7a440acff50] page_fault at ffffffff9f00111e
...
```

From the backtrace of kernel stack, we can infer that `stress` caused a page fault and the kernel handled the fault by trying to map free pages to the process' virtual memory.
Apparently, there were no free pages available, and the kernel failed to reclaim new free pages (`__alloc_pages_slowpath`), and it ended up in `out_of_memory.cold.35`,
where it caused a kernel panic because `panic_on_oom` is enabled. The question is, why was the system low on memory in the first place.

```
crash> kmem -i
                 PAGES        TOTAL      PERCENTAGE
    TOTAL MEM   464508       1.8 GB         ----
         FREE    12961      50.6 MB    2% of TOTAL MEM
         USED   451547       1.7 GB   97% of TOTAL MEM
       SHARED     1615       6.3 MB    0% of TOTAL MEM
      BUFFERS        0            0    0% of TOTAL MEM
       CACHED     2265       8.8 MB    0% of TOTAL MEM
         SLAB    15671      61.2 MB    3% of TOTAL MEM

   TOTAL HUGE        0            0         ----
    HUGE FREE        0            0    0% of TOTAL HUGE

   TOTAL SWAP        0            0         ----
    SWAP USED        0            0    0% of TOTAL SWAP
    SWAP FREE        0            0    0% of TOTAL SWAP

 COMMIT LIMIT   232254     907.2 MB         ----
    COMMITTED   607729       2.3 GB  261% of TOTAL LIMIT
```

We can see that the kernel had no available swap space to store currently unused pages into.
Let's see, which processes allocated so much memory that the system crashed:

```
crash> ps | grep stress
     2348    2287   1  ff38963f73d28000  IN   0.0     7984      828  stress
     2349    2348   2  ff38963f46c00000  IN  25.0   532276   524388  stress
     2350    2348   1  ff38963f48800000  IN  25.0   532276   524388  stress
     2358    2287   1  ff38963f73d29e40  IN   0.0     7984      828  stress
>    2359    2358   0  ff38963f46e65ac0  RU   6.5   270132   135372  stress
     2360    2358   2  ff38963f41920000  UN   4.1   270132    85008  stress
>    2361    2358   2  ff38963f54a35ac0  RU   8.1   270132   170676  stress
>    2362    2358   1  ff38963f419d5ac0  RU   7.9   270132   165616  stress
```

The running `stress` processes occupy more than three quarters of all physical memory by themselves (6th column).
Combined with the rest of the system, this is enough to use up all memory on a machine with only 2 gigabytes of available memory.

Lastly, we can check the kernel log, whether we missed something:

```
crash> log
...
[17449.585352] stress invoked oom-killer: gfp_mask=0x6280ca(GFP_HIGHUSER_MOVABLE|__GFP_ZERO), order=0, oom_score_adj=0
...
[17449.598802] Node 0 active_anon:768kB inactive_anon:1681308kB active_file:376kB inactive_file:84kB unevictable:0kB isolated(anon):0kB isolated(file):0kB mapped:1724kB dirty:0kB writeback:0kB shmem:8740kB shmem_thp: 0kB shmem_pmdmapped: 0kB anon_thp: 1454080kB writeback_tmp:0kB kernel_stack:2928kB pagetables:10280kB all_unreclaimable? yes
[17449.601213] Node 0 DMA free:7280kB min:384kB low:480kB high:576kB active_anon:0kB inactive_anon:7824kB active_file:0kB inactive_file:0kB unevictable:0kB writepending:0kB present:15992kB managed:15360kB mlocked:0kB bounce:0kB free_pcp:0kB local_pcp:0kB free_cma:0kB
[17449.607031] Node 0 DMA32 free:44564kB min:44668kB low:55832kB high:66996kB active_anon:768kB inactive_anon:1673668kB active_file:1052kB inactive_file:188kB unevictable:0kB writepending:0kB present:2080628kB managed:1842672kB mlocked:0kB bounce:0kB free_pcp:372kB local_pcp:348kB free_cma:0kB
...
[17449.672980] Kernel panic - not syncing: Out of memory: system-wide panic_on_oom is enabled
```

Most of the log is omitted, it contains the same backtrace we have seen and info about running tasks at the time of crash,
one interesting thing is that the OOM itself happened because the free memory in the DMA32 zone was below the "min" value:
`DMA32 free:44564kB min:44668kB`. This was unexpected, because the kernel apparently allocated memory for userspace process from the DMA32 zone.

```
crash> kmem -z
NODE: 0  ZONE: 0  ADDR: ff38963fbffb2000  NAME: "DMA"
  SIZE: 4095  PRESENT: 3998  MIN/LOW/HIGH: 96/120/144
  VM_STAT:
        NR_FREE_PAGES: 1820
NR_ZONE_INACTIVE_ANON: 1956
  NR_ZONE_ACTIVE_ANON: 0
NR_ZONE_INACTIVE_FILE: 0
  NR_ZONE_ACTIVE_FILE: 0
  NR_ZONE_UNEVICTABLE: 0
NR_ZONE_WRITE_PENDING: 0
             NR_MLOCK: 0
            NR_BOUNCE: 0
           NR_ZSPAGES: 0
    NR_FREE_CMA_PAGES: 0


NODE: 0  ZONE: 1  ADDR: ff38963fbffb25c0  NAME: "DMA32"
  SIZE: 520157  MIN/LOW/HIGH: 11167/13958/16749
  VM_STAT:
        NR_FREE_PAGES: 11141
NR_ZONE_INACTIVE_ANON: 418417
  NR_ZONE_ACTIVE_ANON: 192
NR_ZONE_INACTIVE_FILE: 47
  NR_ZONE_ACTIVE_FILE: 263
  NR_ZONE_UNEVICTABLE: 0
NR_ZONE_WRITE_PENDING: 0
             NR_MLOCK: 0
            NR_BOUNCE: 0
           NR_ZSPAGES: 0
    NR_FREE_CMA_PAGES: 0


NODE: 0  ZONE: 2  ADDR: ff38963fbffb2b80  NAME: "Normal"
  [unpopulated]

NODE: 0  ZONE: 3  ADDR: ff38963fbffb3140  NAME: "Movable"
  [unpopulated]

NODE: 0  ZONE: 4  ADDR: ff38963fbffb3700  NAME: "Device"
  [unpopulated]
```

Interstingly, the `Normal` zone is not used at all on this system, so OOM in the DMA32 zone makes sense.
I suppose the reason is that those 2 gigabytes of physical memory are mapped to such low addresses, 
that they're usable as DMA memory.

==========================================================================================================

Conclusion: 

The system crashed because there was not enough free memory to handle a page fault of userspace program,
and the kernel was configures to panic at that point. This could be solved by configuring the kernel to kill processes
to recover from OOM (`echo '0' > /proc/sys/vm/panic_on_oom`), adding a swapfile, or allocating more memory to the virtual machine itself.
