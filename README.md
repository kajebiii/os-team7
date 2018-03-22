 OS Team 7 Project

## [Comparing changes](https://github.com/swsnu/os-team7/compare/base...proj1)

## Preprocess
* [arch/arm/include/asm/unistd.h]
* [arch/arm/include/uapi/asm/unistd.h]
* [arch/arm/kernel/calls.S]
* [include/linux/prinfo.h]
* [kernel/Makefile]

## kernel/ptree.c
* Used SYSCALL_DEFINE2 macro

* Input for system call: struct prinfo* buf, int* nr
	- buf : Pointer where to copy process tree information
	- nr : Size of the buffer

* Output for system call: prinfo* buf, int* nr, return value
	- buf : process tree information are copied to buf pointer. Number of entries written in buffer is equal to min(size of buffer, total number of entries on success) 
	- nr : Number of entries written in buffer is stored to nr pointer.
	- return value : If system call successfully returned, the return value will be total number of entries on success. This value may be different from actual number of entries copied. If buf/nr pointer is NULL or value stored in nr is not a positive integer, -EINVAL is returned. If system call cannot write to the buffer, -EFAULT is returned. If kernel doesn't have enough memory and can't afford temporary buffer to hold process informations, -ENOSPC is returned.

* What system call does:
	- Check whether buf/nr pointer is valid, and nr holds positive integer value.
	- Make temporary buffer to read process tree information in kernel memory.
	- Locks tasklist, read process tree information using DFS, and unlocks tasklist.
	- Copy ptree data from temporary buffer to user buffer.
	- Free temporary buffer and returns with appropriate return value.

## testcode/test.c
please add contents
