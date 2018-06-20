# Project 1

* Assigned: 2018-03-07 Wednesday 11:00:00 KST
* Design Review Due: 2018-03-18 Sunday
* **Due: 2018-03-25 Sunday 20:59:59 KST**

## Introduction

This assignment includes implementing a new system call in Linux. It returns the process tree information in depth-first-search order.

This is a team project. Each team will have access to its own GitHub repository (e.g. `swsnu/os-team1`) for collaboration and submission. Your final codes and `README.md` document have to be committed into the _proj1_ branch for submission. Start from [kernel source](https://github.com/swsnu/linux-3.10-artik) and make incremental changes to get things done.

## 1. Writing `ptree` System Call (45 pts.)

The system call you write should take two arguments and return the process tree information in a depth-first-search order.

The prototype for your system call will be:
```c
int ptree(struct prinfo *buf, int *nr);
```

You should define `struct prinfo` as:
```c
struct prinfo {
  long state;             /* current state of process */
  pid_t pid;              /* process id */
  pid_t parent_pid;       /* process id of parent */
  pid_t first_child_pid;  /* pid of oldest child */
  pid_t next_sibling_pid; /* pid of younger sibling */
  long uid;               /* user id of process owner */
  char comm[64];          /* name of program executed */
};
```
in `include/linux/prinfo.h` as part of your solution.

The argument `buf` points to a buffer for the process data, and `nr` points to the size of this buffer (number of `struct prinfo` entries). The system call copies at most that many entries to the buffer in pre-order and stores the number of entries actually copied to variable pointed by `nr`.

`pid_t` value for a non-existing entity is 0. For example, `first_child_pid` should be 0 if the process does not have a child.

Each system call must be assigned a number. Your system call should be assigned number **380**.

### Return Value

Your system call should return the total number of entries on success (this may be bigger than the actual number of entries copied).

Your code should handle errors that can occur but not handle any errors that cannot occur. At a minimum, your system call should detect and report the following error conditions:
* `-EINVAL`: if `buf` or `nr` are null, or if the number of entries is less than 1
* `-EFAULT`: if `buf` or `nr` are outside the accessible address space.
The referenced error codes are defined in `include/uapi/asm-generic/errno-base.h`

### tasklist_lock

Linux maintains a list of all processes in a doubly linked list. Each entry in this list is a `task_struct` structure, which is defined in `include/linux/sched.h`. When traversing the process tree data structures, it is necessary to prevent the data structures from changing in order to ensure consistency. For this purpose the kernel relies on a special lock, the `tasklist_lock`. You should grab this lock before you begin the traversal, and only release the lock when the traversal is completed. While holding the lock, your code may not perform any operations that may result in a sleep, such as memory allocation, copying of data into and out from the kernel etc. Use the following code to grab and then release the lock:

```c
read_lock(&tasklist_lock);
/* do the job... */
read_unlock(&tasklist_lock);
```
## 2. Test your new system call (10 pts.)

Write a simple C program which calls `ptree` system call. Your program should print the entire process tree in pre-order using tabs to indent children with respect to their parents. For each process, it should use the following format for program output:
```c
printf("%s,%d,%ld,%d,%d,%d,%d\n", p.comm, p.pid, p.state,
  p.parent_pid, p.first_child_pid, p.next_sibling_pid, p.uid);
```

You can invoke `ptree` system call using `syscall` function. (See [here](https://linux.die.net/man/2/syscall) for details.)

### Example program output

```
systemd,1,1,0,156,2,0
  systemd-journal,156,1,1,0,185,0
  systemd-udevd,185,1,1,0,484,0
  syslogd,484,1,1,0,495,0
    ...
  deviced,802,1,1,1612,857,0
    systemctl,1612,64,802,0,1613,0
    systemctl,1613,64,802,0,1614,0
    systemctl,1614,64,802,0,31175,0
    ...
kthreadd,2,1,0,3,0,0
  ksoftirqd/0,3,1,2,0,5,0
  kworker/0:0H,5,1,2,0,6,0
  kworker/u8:0,6,1,2,0,7,0
```

### Compiling test program


```bash
arm-linux-gnueabi-gcc -I<your kernel path>/include test.c -o test
```

### Running test program

```bash
sdb push test /root/test
```

(Execute `root` command if `Permission denied` error occurs.)

In debug console, (using `screen`, or PuTTY)

```bash
/root/test
```

**Save your C program as : `test/test_ptree.c` in your team repository.**

## We're Here to Help You

Any troubles? Questions on [issue board](https://github.com/swsnu/osspr2018/issues) is more than welcome. Discussions between students are also encouraged.

Start early, be collaborative, and still most importantly, have fun!
