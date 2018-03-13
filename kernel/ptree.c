#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>

void dfs(struct task_struct *now, int dep) {
	printk("[ptree] %s,%d\n", now->comm, now->pid);
	struct list_head *p;
	struct task_struct *t;
	/*
	list_for_each_entry(t, &now->children, children) {
		printk("[ptree] %s,%d\n", t->comm, t->pid);
	}
	*/
	list_for_each(p, &now->children) {
		t = list_entry(p, struct task_struct, sibling);
		//if(dep == 0) dfs(t, dep+1);
		if(t != NULL) dfs(t, dep+1);
	}
}
SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	struct task_struct *root = &init_task;
	printk("[ptree] %d\n", root->pid);
	printk("[ptree] hello, world!\n");
	dfs(root, 0);
	return 1;
}
