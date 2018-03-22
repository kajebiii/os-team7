#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/slab.h>

static int get_next_sibling(struct task_struct *now){
	if(now->sibling.next == &(now->real_parent->children)) return 0;
	return list_entry(now->sibling.next, struct task_struct, sibling)->pid;
}

static int ptree_dfs(struct task_struct *now, struct prinfo* out, struct prinfo *end) {
	int re = 0;
	struct list_head *p;
	struct task_struct *t;
	
	if (now != &init_task){
		re = 1;
		if(out < end){
			strcpy(out->comm, now->comm);
			if (list_empty(&now->children)) out->first_child_pid = 0;
			else out->first_child_pid = list_entry(now->children.next, struct task_struct, sibling)->pid;
			out->next_sibling_pid = get_next_sibling(now);
			out->parent_pid = now->real_parent->pid;
			out->pid = now->pid;
			out->state = now->state;
			out->uid = task_uid(now);
		}
		else printk("[ptree]    end of buffer\n");
	}

	list_for_each(p, &now->children) {
		t = list_entry(p, struct task_struct, sibling);
		re += ptree_dfs(t, out+re, end);
	}
	return re;
}


SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	int list_size;
	int err = 0;
	int num_of_proc;
	int remain_bytes, return_value;
	void *kbuf, *ubuf, *kbuf_origin;

	if (buf == NULL || nr == NULL) return -EINVAL;

	if ((err = get_user(list_size, nr)) < 0) return err;
	if (list_size <= 0) return -EINVAL;

	if (access_ok(VERIFY_WRITE, buf, list_size * sizeof(struct prinfo)) == 0) return -EFAULT;

	kbuf_origin = kbuf = kmalloc(list_size*sizeof(struct prinfo), GFP_KERNEL);
	if (kbuf == NULL) return -ENOSPC;
	
	read_lock(&tasklist_lock);
	num_of_proc = ptree_dfs(&init_task, kbuf, kbuf + list_size * sizeof(struct prinfo));
	read_unlock(&tasklist_lock);

	if (num_of_proc < 0) return num_of_proc;
	if (list_size > num_of_proc) list_size = num_of_proc;

	ubuf = buf;
	remain_bytes = list_size * sizeof(struct prinfo);
	return_value = num_of_proc;

	kbuf += remain_bytes; ubuf += remain_bytes;
	while((remain_bytes = copy_to_user(ubuf - remain_bytes, kbuf - remain_bytes, remain_bytes)) > 0);

	if(remain_bytes < 0) return_value = remain_bytes;
	if((err = put_user(list_size, nr)) < 0) return_value = err;

	kfree(kbuf_origin);
	return return_value;
}
