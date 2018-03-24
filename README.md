# OS Team 7 Project 1 README

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

## kernel/ptree.c
* Used SYSCALL_DEFINE2 macro

* Input for system call: struct prinfo* buf, int* nr
	- buf : Pointer where to copy process tree information
	- nr : Size of the buffer

* Output for system call: prinfo* buf, int* nr, return value
	- buf : process tree information are copied to buf pointer. Number of entries written in buffer is equal to min(size of buffer, total number of entries on success).
	- nr : Number of entries written in buffer is stored to nr pointer.
	- return value : If system call successfully returned, the return value will be total number of entries on success. This value may be different from actual number of entries copied. If buf/nr pointer is NULL or value stored in nr is not a positive integer, -EINVAL is returned. If system call cannot write to the buffer, -EFAULT is returned. If kernel doesn't have enough memory and can't afford temporary buffer to hold process informations, -ENOSPC is returned.

* What system call does:
	- Check whether buf/nr pointer is valid, and nr holds positive integer value.
	- Make temporary buffer to read process tree information in kernel memory.
	- Locks tasklist, read process tree information using DFS, and unlocks tasklist.
	- Copy ptree data from temporary buffer to user buffer.
	- Free temporary buffer and returns with appropriate return value.

## test/test_ptree.c
* What test code does:
	- Repeatedly calls syscall(380, data, &size) function, until it gets entire process tree properly. If program fails to get whole process tree, increase buffer size to be enough to contain whole processes.
	- Print visualized process tree.
	- Free pointers, and returns.

## Investigation of the process tree
* Process tree is the way to show all running processes using tree format. In this tree, each node represents one process. The parent of each node is parent process, which is one that created the process. 'pstree' command can show process tree in Linux.
* In Linux OS, there are slightly different two types of parent process, 'Real parent' and 'Parent'. Real parent process is same with original definition. It means the process that created child process. Parent process is the process that receives SIGCHLD signal. These two process coulc be different. In process tree, real parent process becomes parent node. 
* Process information is stored with "struct task_struct" type in kernel. This struct contains lots of informations about the process such as process id, parent process id, sibling process id, process state, and so on. We can follow these struct using parent/children pointer, and build process tree effectively by using DFS.

## Any lessons learned
* We learned how to use Git and Github properly. We used branch and push/pull process properly and it helped our work a lot.
* We learned about the structure how processes are stored in Linux system. We learned about task_struct structure and figured out which information they contains. We learned how to build linked list using list_head structure and related macros. Furthermore, we could found some useful informations for further assignments.
* We learned how to add system calls in Linux system. We learned about modifying appropriate files, using useful macros in low-level programming. 
