# OS Team 7 Project 3 README

## How to build kernel
The way to build kernel does not differ from original kernel. Type `./build` in the root folder, and kernel will be built. Or you can type `make build` in the "test" folder.

## Flashing the Device
The way to flash the device is the same as original.

After the building is done.

1. Open the debug console. (Use `screen` command or PuTTY)
1. Hit reset button on the device.
1. Push and hold power button for 1 second.
1. You'll see `Hit any key to stop autoboot`. Press any key.
1. Type `thordown` and <kbd>Enter</kbd>. The device will enter thordown mode.

Just type `lthor image.tar` in the root folder, or `make lthor` in the "test" folder.

## How to execute test code
```bash
HOST$ XXX
```
* It means "type XXX at the terminal of your Ubuntu-PC."

```bash
BOARD$ YYY
```
* It means "Type YYY at the terminal of Artik 10."

After logging in, you have to execute 
```bash
BOARD$ direct_set_debug.sh --sdb-set
```
to enable sdb. 


In the "test" folder, type 
```bash
HOST$ make clean
HOST$ make
HOST$ make run
```
Then, It will compile test codes and transfer our test excutable files between the device and your PC.

And if you execute
```bash
BOARD$ chmod 777 manyWhiles
BOARD$ ./manyWhiles
```
above command, it will execute 16 ./while in background;

And if you want to execute our trial program
```bash
BOARD$ ./trial [ITER]
```
ITER is iteration number of naive Trial Division.
Trial program will calculate average wall-clock time by weights (1 to 20). And result will export to "output.txt" in "root/".

In ther "test/shellScript" folder, type
```bash
HOST$ ./proc_kmsg.sh
```
will show printk messages. We printed "load balance visited" and "task move info" using prink function.

Type
```bash
HOST$ ./sched_debug.sh
```
will show scheduler debug information. We printed wrr_weight_sum per wrr_rq. And each task's wrr_weight, wrr_timeslice, and policy.

If you want to see only our trial program task's infomation, type
```bash
HOST$ ./find_trial.sh
```
adove command. 

You will need to open two or many terminal.


## [Comparing changes](https://github.com/swsnu/os-team7/compare/base...proj3)

## Preprocess
* Add new system call
	* [arch/arm/include/asm/unistd.h](https://github.com/swsnu/os-team7/blob/proj3/arch/arm/include/asm/unistd.h)
	* [arch/arm/include/uapi/asm/unistd.h](https://github.com/swsnu/os-team7/blob/proj3/arch/arm/include/uapi/asm/unistd.h)
	* [arch/arm/kernel/calls.S](https://github.com/swsnu/os-team7/blob/proj3/arch/arm/kernel/calls.S)
* Add new scheduler option SCHED_WRR
	- Define SCHED_WRR as 6
		* [include/uapi/linux/sched.h](https://github.com/swsnu/os-team7/blob/proj3/include/uapi/linux/sched.h)
	- Serve as the default scheduling policy
		* [include/linux/init_task.h](https://github.com/swsnu/os-team7/blob/proj3/include/linux/init_task.h)
		* [kernel/kthread.c](https://github.com/swsnu/os-team7/blob/proj3/kernel/kthread.c)
	- Add wrr_rq sturct
		* [kernel/sched/sched.h](https://github.com/swsnu/os-team7/blob/proj3/kernel/sched/sched.h)
	- Add sched_wrr_entity struct
		* [include/linux/sched.h](https://github.com/swsnu/os-team7/blob/proj3/include/linux/sched.h)
	- Set wrr_sched_class by prio, policy / init_wrr_rq
		* [kernel/sched/core.c](https://github.com/swsnu/os-team7/blob/proj3/kernel/sched/core.c)
	- Change rt_sched_class's next
		* [kernel/sched/rt.c](https://github.com/swsnu/os-team7/blob/proj3/kernel/sched/rt.c)
	- For debug our scheduler
		* [arch/arm/configs/artik10_defconfig](https://github.com/swsnu/os-team7/blob/proj3/)

### kernel/sched/wrr.c (About wrr_sched_class)

* Data structure
	- wrr_rq
		* struct list_head `run_list` : head of the linked list
		* unsigned int wrr_weight_sum : sum of weights of tasks in rq
		* unsigned long next_balance : time that we should execute next load balance

	- sched_wrr_entity
		* int `weight` : weight of sched_wrr_entity
		* int `time_slice` : time_slice of sched_wrr_entity
		* struct list_head `run_list` : Linked list in rq

## wrr_sched_class functions

* init_wrr_rq
	- Initialize run_list in wrr_rq

* enqueue_task_wrr
	- Add wrr_entity's run_list at tail of wrr_rq's linked list
	- Increase nr_running

* dequeue_task_wrr
	- Delete wrr_entity's run_list in wrr_rq's linked list
	- Initialize wrr_entity's run_list
	- Decrease nr_running

* yield_task_wrr
	- Move rq's current running wrr_entity to tail of wrr_rq's linked list

* pick_next_task_wrr
	- Pick first entry of the linked list (wrr task queue). 
	- If there was no task (list was empty) return NULL. Else return task_struct of the wrr_entity

* select_task_rq_wrr
	- Used rcu_read_lock
	- return cpu which can have this task and has smallest weight sum
	- This function does passive load balance.

* task_tick_wrr
	- If policy of task is not SCHED_WRR returns
	- Decrease wrr_entity's time_slice
	- If time_slice is zero and needs context switching (There are 2 or more tasks in run queue), move task to back of the queue and reschedule the task.

* get_rr_interval_wrr
	- return wrr_entity's time_slice

## load balancing functions

* run_rebalance_domains_wrr
	- Calculate min_cpu and max_cpu, which has minimum/maximum weight sum
	- Among tasks in max_cpu, find tasks that can be moved to min_cpu that satisfies following conditions:
		* Can be migrated to min_rq
		* After moved, min_rq weight sum < max_rq weight_sum
		* Tasks are not currently running
		* Among those tasks, choose task with biggest weight
	- If there is task with above condition, move that task from min_rq to max_rq

* init_sched_wrr_class
	- Registers handler for SCHED_SOFTIRQ_WRR signal to call run_rebalance_domains_wrr

* trigger_load_balance_wrr
	- Locks wrr_balancing, to prevent lots of cpus call load balance at once.
	- If enough time passed from previous load balance, raise SCHED_SOFTIRQ_WRR signal to run load_balance function.
	- Unlocks wrr_balancing.


## kernel/sched/wrr.c (About two system call)
* sched_setweight(pid_t pid, int weight)
	- Used SYSCALL_DEFINE2 macro
	- Set weight of the task with input pid.
	- Get pid number and weight as an input.
	- If function gets valid pid and weight, returns weight of task specified by pid. Else (pid < 0, there is no task with that pid, weight range is invalid or user has no permission to change weight), returns -EINVAL.

* sched_getweight(pid_t pid)
	- Used SYSCALL_DEFINE1 macro
	- Get pid number as an input.
	- If function gets valid pid, returns weight of task specified by pid. Else (pid < 0 or there is no task with that pid), returns -EINVAL.
	
## concurrency control
 * Lock related with scheduler
 	- task_struct.pi_lock : Lock of task_struct
 	- rq.lock : Lock of runqueue
	- rcu_read_lock : Accquire when access cpu-related information. Almost every where.
 * Assume that every callback in sched_class will call with accquired rq's lock and rcu_read_lock
 * In setweight system call, we should get rq lock of given task
 	- core.c's task_rq_lock : accquire given task_struct's pi_lock and corresponds runqueue's lock and return runqueue
 * In load_balance, when move task, we should get source and destination run queue's lock. 
 	- Use core.c's double_rq_lock to get both lock with prevent deadlock
	
## test/trial.c, test/while.c
* What test code does:
	- test/trial.c
		* For each weight from 1~20, calculate (real) time elapsed to factorize big prime(5e6+11 = 5000011).
		* Repeats ITER times (which can be given as input), and prints average value.
	- test/while.c
		* Set random weights, and executes while loop.
		* This code does nothing and holds CPU.
	- We executed while.c 32 times and executed trial.c, to test load balance.

## Any lessons learned
* We learned the structure of Linux scheduler, and how to add new scheduler in it. We had to add new entity and rq for, and implement necessary functions and structs to implement it.
* We learned how to debug in kernel code, using printk and some other ways. We experienced lots of kernel panic this time, and could debug those using our methods.
* We learned about mechanism of load balancing in Linux kernel, and how to implement our own load balancing mechanisms. We learned how to use jiffies to check time to load balance, and only one cpu can do it.
