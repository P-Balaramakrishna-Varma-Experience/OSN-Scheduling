## Syscall trace.

### New data added.
* *int* variable **Trace** in *struct proc* to store which all syscalls to trace. (in the encoding mentioned for mask in assignment pdf).
* An array of strings **SysCallNames** where index *i* stores the name of syscall with number *i*, added in *syscall* function.
* An array of numbers **SysCallNum** where index *i* stores the no of args for syscall *i* , added in *syscall* function.

### Functional changes
* The default behaviour of the system is to trace all system calls encoded in *Trace* in struct proc for all proccess. All changes to me made to get is behaviour is in *syscall* function. Which is the entry and exit point of every system call. So we have both arguments and return value there. 
* We intialize the *Trace* variable to *0* during the creation of the process. So that the process does not trace any system call.
* All we do in *trace syscall* is to change the value of *Trace* in *struct proc*.
* In the User program *strace* all we do is call the syscall strace with arguments given via commandline (argv). And then exec to the command as given in arguments via commandline.




## Schedulers
### FCFS
* A new variable *start_time* in *struct proc* in *proc.h*. This is used to decide which process is to be scheduled.
* To make it non-preemptive we remove *yield();* from usertrap and kerneltrap functions in trap.c, in the if statement used to detect timer interrupt.
* In the scheduler function. Each time we go through the entire *proc* array (iterating) to find the process which has arrived first.
* In the iteration we only consider the process which are runnable. Other process are ignored.
* We keep track of the minimum start time process up until then. This minimum process is also locked until a new minimum is process has arrived.

### PBS
* The code for scheduler is exactly same as for fcfs with a small difference (1 line code change). In the if statement to check if we have better process to scheduler than the current best. We use a function which compare two process and outputs the result of comparison.
* This function of comparison in explained the hints of Assignment pdf. 
* We do the same thing done in FCFS to disable preemption on trimmer interrupt.
* New variables *running_time* *sleeping_time* *Static_priority* *start_time (time created)* *TimesScheduled* in *struct proc* in *proc.h*.
* Static_priority it is intialized to 60 at the creation of process. This value is changed/set to another value only by *set_priority* syscall. This variable stores the static priority of the process.
* Times_Scheduled it is intialized to 0 at the creation of a process. The value is incremented by 1 each time it is scheduled (in scheduler function). This variable stores the number of times the process is scheduled.
* Time_created it is intialized to *ticks* at the time of creation of a process. It is then never modified. This variable stores when the process was created.
* running_time, stores how many ticks the process has been running from last time it was scheduled (to be only read in scheduler function. It is initalized -1 during process creation and in set_priority syscall (to indicate niceness = 5). When the process is being scheduled the time(ticks) is stored. The process then either exits or sleeps. In case of exits nothing to be done. In case it sleeps *running_time = ticks - running_time*. This code to be added in sleep function.
* sleeping_time, stores how many ticks the process has been sleeping from last time it was scheduled (to be only read in scheduler function. It is initalized -1 during process creation and in set_priority syscall (to indicate niceness = 5). When the process calls sleep function time(ticks) is stored. The process then is made runnable in a wakeup function from some other process. Over here *sleeping_time = ticks - sleeping_time*. This code to be added in wakeup function function.
* *set_priority* syscall is trivial to implement. We search the proc  for the process (given pid) and change the static_priority, running_time, sleeping_time variables. We preempt if the priority has increased.

### MLFQ
* New variables *time_added*, *priority number* and *no_ticks* in *struct proc* in proc.h.
* **time_added** stores the time at which it was added to a queue. It is set during creation of process (fork) or when completed I/O (sleeping->runnable) or expiry of time slice. It is used to determine the head of a queue.
* **priority number** stores to which queue a process belongs to. It is set to 0 during process creation. It then modified via upgradation and degradation policies.
* **no_tick** it contains number of ticks elapsed from the time it was scheduled. It is valid only when the process is running. It is set to 0 whenever it is added to a queue. It is incremented each time a timer interrupt occurs.
* The preemptive nature. A process is always created using a fork syscall. In the fork call we can check if the new process has a higher priority compared to the running process. If yes then we yield the current process and call the scheduler which would then schedule the new process since it has higher priority. If no nothing is done there is no need to preempt.
* When the *no_ticks* for a process exceeds the max_wait time in its queue. We upgrade it by increasing it's priority (moving a higher priority queue). *no_ticks* is set to 0 and *time_added* is set to current ticks. In case the process in queue no 4 we just yield and call scheduler (round robin)
* Before scheduling we check if the waiting time of every process is less then the max_wait time for a process in it's queue. All the process which are waiting from long time (more than expected) are promoted to a higher queue and it's *time_added* is set to current ticks.
* Scheduling. We find the head of queue 0. If present we use that. If head of queue 0 not present(empty). We find the head of queue 1. If present we use that. If not present (empty queue). We find head of queue 3 ... . I all the queue 0,1,2,3 are empty then run the process in queue 4 in round robin.
* Finding head of a queue (0,1,2,3). Head the is the process which has the least value for *time_added*.




## ProcDump.
* PBS, MLFQ only change needed is the printf statement in procdump function in proc.c 
* rtime, ntime, pid, state are already there, no extra work needed. 
* wtime has to be computed using ctime, rtime, etime/ticks.





## Exploiting MLFQ
* The following scheduling favours I/O bound process. 
* So an I/o bound application is taking exploiting this scheduler implicitly.
* We can take advantage of this scheduler to make a CPU bound process to run in high priority queue.
* We can call sleep(10); to voluntarily relinquish CPU after completing a set of instructions that take little less time then a tick Since the process did not use it's entire time slice it is put at the end of the highest priority queue. It is not demoted.





## Performance

### Comparsion. 
* (rtime, wtime), avarage values, units are ticks (timmer iterrupt lenght). (Format used).
* since MLFQ is expected to run only on single cpu. We should run all schedulers with single cpu.
* RR  (169, 17)
* FCFS (158, 35)
* PBS (136, 18)
* MLFQ (165, 17)

### Tabulation
* For 2 cpus
  * RR (127, 15)
  * FCFS (69, 35)
  * PBS (115, 18)
  * MLFQ not runnable.

* For 3 Cpus
  * RR (116, 17)
  * FCFS (49, 41)
  * PBS (107, 28)
  * MLFQ not runnable