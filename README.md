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
to enable sdb. And if you execute
```bash
BOARD$ cat /proc/kmsg | grep "wrr"
```
above command, you can see our logs in wrr.c

In the "test" folder, type 
```bash
HOST$ make clean
HOST$ make
HOST$ make run
```
Then, It will compile test codes and transfer our test excutable files between the device and your PC.

```bash
HOST$ sdb shell
HOST$ (sdb) > /root/selector [STARTING INTEGER]
```
Above command will execute `selector`. 
```bash
HOST$ sdb shell
HOST$ (sdb) > /root/trial [INTERGER IDENTIFIER]
```
Above command will execute `trial`.

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

## kernel/sched/wrr.c (About wrr_sched_class)

* Data structure
	- wrr_rq
		* struct list_head `run_list` : head of the linked list
		* TODO
	- sched_wrr_entity
		* int `weight` : weight of sched_wrr_entity
		* int `time_slice` : time_slice of sched_wrr_entity
		* struct list_head `run_list` : Linked list in rq
		* struct wrr_rq `wrr_rq` : wrr_rq of struct_wrr_entity

### wrr_sched_class functions

* init_wrr_rq
	- Initialize run_list in wrr_rq

* enqueue_task_wrr
	- Set wrr_entity's wrr_rq to `rq->wrr`
	- Add wrr_entity's run_list at tail of wrr_rq's linked list
	- Increase nr_running

* dequeue_task_wrr
	- Set wrr_entity's wrr_rq to `NULL`
	- Delete wrr_entity's run_list in wrr_rq's linked list
	- Initialize wrr_entity's run_list
	- Decrease nr_running

* yield_task_wrr
	- Move rq's current running wrr_entity to tail of wrr_rq's linked list

* pick_next_task_wrr
	- Pick first entry of the linked list. (if the list is empty, return `NULL`)
	- return task_struct of the wrr_entity

* select_task_rq_wrr
	- Used rcu_read_lock
	- return cpu which have smallest weight sum

* task_tick_wrr
	- Decrease wrr_entity's time_slice
	- if time_slice is zero, dequeue and enqueue the task

* get_rr_interval_Wrr
	- return wrr_entity's time_slice

## kernel/sched/wrr.c (About two system call)
* sched_setweight(pid_t pid, int weight)
	- Used SYSCALL_DEFINE2 macro
	- TODO
* sched_getweight(pid_t pid)
	- Used SYSCALL_DEFINE1 macro
	- TODO

## test/trial.c
* What test code does:
	- TODO

## Any lessons learned
* TODO
