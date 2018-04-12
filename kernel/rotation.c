#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

SYSCALL_DEFINE1(set_rotation, int, degree)
{
	return 380;
}

SYSCALL_DEFINE2(rotlock_read, int, degree, int, range)
{
	return 381;
}

SYSCALL_DEFINE2(rotlock_write, int, degree, int, range)
{
	return 382;
}

SYSCALL_DEFINE2(rotunlock_read, int, degree, int, range)
{
	return 383;
}

SYSCALL_DEFINE2(rotunlock_write, int, degree, int, range)
{
	return 385;
}

void exit_rotlock(struct task_struct *tsk)
{
	//
}
