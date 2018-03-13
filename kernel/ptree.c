#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	struct task_struct root = init_task;
	printk("[ptree] %d\n", root.pid);
	printk("[ptree] hello, world!\n");
	return 1;
}
