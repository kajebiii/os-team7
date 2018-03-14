#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/slab.h>

static int ptree_dfs(struct task_struct *now, struct prinfo* out, int *idx, int max_len, pid_t next_sibling, pid_t par) {
	int re = 0, err;
	struct list_head *p;
	struct task_struct *t;
	pid_t ns = 0;
	printk("[ptree] %s,%d\n", now->comm, now->pid);
	
	if (now != &init_task){
		re = 1;
		if(*idx < max_len){
			strcpy(out[*idx].comm, now->comm);
			if (list_empty(&now->children)) out[*idx].first_child_pid = 0;
			else out[*idx].first_child_pid = list_entry(now->children.next, struct task_struct, sibling)->pid;
			out[*idx].next_sibling_pid = next_sibling;
			out[*idx].parent_pid = par;
			out[*idx].pid = now->pid;
			out[*idx].state = now->state;
			out[*idx].uid = task_uid(now);
			(*idx)++;
		}
		else printk("[ptree]    end of buffer\n");
	}

	list_for_each(p, &now->children) {
		t = list_entry(p, struct task_struct, sibling);
		ns = 0;
		if(p->next != &(now->children)) {
			ns = list_entry(p->next, struct task_struct, sibling)->pid;
		}
		re += ptree_dfs(t, out, idx, max_len, ns, now->pid);
	}
	return re;
}


SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	int list_size;
	int err = 0;
	int num_of_proc;
	int remain_bytes, write_bytes;
	void *kbuf, *ubuf, *kbuf_origin;
	int idx = 0;

	if (buf == NULL || nr == NULL) return -EINVAL;

	if ((err = get_user(list_size, nr)) < 0) return err;
	if (list_size <= 0) return -EINVAL;

	if (access_ok(VERIFY_WRITE, buf, list_size * sizeof(struct prinfo)) != 0) return -EFAULT;

	kbuf_origin = kbuf = kmalloc(list_size*sizeof(struct prinfo), GFP_KERNEL);

	read_lock(&tasklist_lock);
	num_of_proc = ptree_dfs(&init_task, kbuf, &idx, list_size, -1, -1);
	read_unlock(&tasklist_lock);

	if (num_of_proc < 0) return num_of_proc;

	ubuf = buf;
	remain_bytes = (list_size<num_of_proc?list_size:num_of_proc) * sizeof(struct prinfo);
	while(remain_bytes > 0){
		write_bytes = copy_to_user(ubuf, kbuf, remain_bytes);
		if(write_bytes < 0) return write_bytes;
		write_bytes = remain_bytes - write_bytes;
		ubuf += write_bytes;
		kbuf += write_bytes;
		remain_bytes -= write_bytes;
	}

	kfree(kbuf_origin);

	return num_of_proc;
}
