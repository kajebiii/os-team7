# OS Team 7 Project 2 README

## How to build kernel
The way to build kernel doesn't differ from original kernel. Type `./build` in the root folder, and kernel will be builded. Or you can type `make build` in the "test" folder.

## Flashing the Device
The way to flash the device doesn't differ from original.

After the building is done.

1. Open the debug console. (Use `screen` command or PuTTY)
1. Hit reset button on the device.
1. Push and hold power button for 1 second.
1. You'll see `Hit any key to stop autoboot`. Press any key.
1. Type `thordown` and <kbd>Enter</kbd>. The device will enter thordown mode.

Just type `lthor image.tar` in the root folder, or `make lthor` in the "test" folder.

## How to execute test code
After logging in, you have to execute `direct_set_debug.sh --sdb-set` to enable sdb.

In the "test" folder, type `make clean` and `make`.

Then type `make run`, will show result of our test code(ptree) and result of `pstree 0`


## [Comparing changes](https://github.com/swsnu/os-team7/compare/base...proj1)

## Preprocess
* [arch/arm/include/asm/unistd.h]
* [arch/arm/include/uapi/asm/unistd.h]
* [arch/arm/kernel/calls.S]
* [include/linux/prinfo.h]
* [kernel/Makefile]

## kernel/rotation.c
* Used SYSCALL_DEFINE2 macro

* There are 5 system calls in this file.
	- set_rotation : Set the current rotation degree of the device.
		* Input: int degree(rotation degree that user wants to set.) 
		* Output: Return total # of wakeup lock, -EINVAL for invalid range.
	- rotlock_read : Get read lock for designated angle range.
		* Input: int degree, int range (User wants to get read lock for [degree-range, degree+range].)
		* Output: 0 for successful lock, -EINVAL for invalid range.
	- rotlock_write: Get write lock for designated angle range.
		* Input: int degree, int range (User wants to get write lock for [degree-range, degree+range].)
		* Output: 0 for successful lock, -EINVAL for invalid range.
	- rotunlock_read: Unlock read lock for designated angle range.
		* Input: int degree, int range (User wants to release read lock for [degree-range, degree+range].)
		* Output: 0 for successful unlock, -EINVAL for invalid range / Non locked range.
	- rotunlock_write: Unlock write lock for designated angle range.
		* Input: int degree, int range (User wants to release write lock for [degree-range, degree+range].)
		* Output: 0 for successful unlock, -EINVAL for invalid range / Not locked range.

* What system call does (set_rotation):
	- Check whether degree is valid.
	- Get mutex for degree, and change SYSTEM_DEGREE value.
	- Find available locks by waking up waiting reading/waiting locks.

* What system call does (rotlock_read, rotlock_write)
	- Check whether range is valid.
	- Generate lock struct and store informations.
	- Add the lock struct to waiting queue.
	- Wait until gets the lock, and returns 0.

* What system call does (rotunlock_read, rotunlock_write)
	- Check whether range is valid.
	- Check whether there is lock by caller process in range. If not, return -EINVAL.
	- Remove lock from each entry in the array.
	- Find locks that became available by unlocking this lock, and returns 0.

* lock_info struct 
	- contains informations about lock
	- contains completion

* Completion
	- FIFO semaphore
	- resolve race condition
	- call wait_for_completion when the task wants to acquire lock 
	- wait_for_completion_interruptible()
		* Wait for completion call
		* returns -ERESTARTSYS when signal is pending
		* return 0 when completion is called

* Data structure
	- Acquire check arrays
		* write_acq_chk[i] : Number of of acquired write lock contains degree i
		* read_acq_chk[i] : Number of of acquired read lock contains degree i
	- Wait node list : linked list of wait locks
	- Acquire node list : linked list of wait locks
	- A mutex lock controls those data structures to prevent simultaneous accessing.

* Finding acquirable lock
	- policy
		* write lock has higher priority
		* Find waiting write lock contains current degree. If it can grab lock now, acquire write lock and return. If there are at least one waiting write lock contains current degree, do not acquire any waiting read lock. Otherwise, find waiting read lock contains current degree. If possible, acquire read lock immediately.

* exit_rotlock: This function is always called when the process was terminated.
	- Check whether there is waiting lock / acquired lock in each range. If so, delete those locks from queue.
	- Find locks that became available by unlocking these locks, and gives them lock if possible.

## test/selector.c
* What test code does:
	- Get write rotation lock for range [0,180].
	- Open file and write integer to that file.
	- Unlock write rotation lock.
	- Increment integer by one, sleep for one second, and repeat above process.

## test/trial.c
* What test code does:
	- Get read lock for range [0,180].
	- Open file and read integer from that file.
	- Calculates factorization result for the integer.
	- Print the result, and unlock read lock.
	- Sleep for one second, and repeat above process.

## Any lessons learned
* We learned how to use mutex to prevent concurrency issues. We learned about lots of concurrency issues such as race condition, deadlock, and so on. We figured out how to handle those issues properly in kernel code.
* We learned how to handle starvation issues for locks. We learned that write starvation can occur because of lots of read locks, and it should be handled by giving some priority to waiting write locks.
* We learned how to make processes wait in valid way. We learned that process should not just sleep, and they should be waked up when the conditions are fulfilled. We learned how to make processes wait in valid way, not making the kernel to crash.
* We learned about exit function, which should be called at the termination of process. We learned that process can always be terminated in appropriate/inappropriate way, and exit function should handle them in a valid way. We handled those cases in exit_rotlock function.
