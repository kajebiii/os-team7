#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>

static int ptree_dfs(struct task_struct *now, struct prinfo** out, struct prinfo* end, pid_t next_sibling, pid_t par) {
	int re = 0, err;
	struct list_head *p;
	struct task_struct *t;
	struct prinfo info;
	pid_t ns = 0;
	printk("[ptree] %s,%d\n", now->comm, now->pid);
	
	if (now != &init_task){
		re = 1;
		strcpy(info.comm, now->comm);
		if (list_empty(&now->children)) info.first_child_pid = 0;
		else info.first_child_pid = list_entry(now->children.next, struct task_struct, sibling)->pid;
		info.next_sibling_pid = next_sibling;
		info.parent_pid = par;
		info.pid = now->pid;
		info.state = now->state;
		info.uid = task_uid(now);
		if(*out != end){
			err = copy_to_user(&info, *out, sizeof(struct prinfo));
			if(err < 0) return err;
			(*out)++;
		}
	}

	list_for_each(p, &now->children) {
		t = list_entry(p, struct task_struct, sibling);
		ns = 0;
		if(p->next != &(now->children)) {
			ns = list_entry(p->next, struct task_struct, sibling)->pid;
		}
		re += ptree_dfs(t, out, end, ns, now->pid);
	}
	return re;
}


SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	int list_size;
	int err = 0;
	int num_of_proc;

	if (buf == NULL || nr == NULL) return -EINVAL;

	if ((err = get_user(list_size, nr)) < 0) return err;
	if (list_size <= 0) return -EINVAL;

	if (access_ok(VERIFY_WRITE, buf, list_size * sizeof(struct prinfo)) < 0) return -EFAULT;

	read_lock(&tasklist_lock);
	num_of_proc = ptree_dfs(&init_task, &buf, buf + list_size, -1, -1);
	read_unlock(&tasklist_lock);

	if (num_of_proc < 0) return num_of_proc;
	return num_of_proc - 1; 
}
