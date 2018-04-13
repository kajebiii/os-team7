#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/list.h>

int SYSTEM_DEGREE = 0;

DEFINE_MUTEX(rotlock_mutex);

struct lock_info{
	struct task_struct *proc;
	int degree, range;
	int mode; // 1 : read, 2 : write
}

struct wait_node{
	struct completion comp;
	struct lock_info* lockinfo;
	struct list_head list;
};

static LIST_HEAD(wait_node_list_read);
static LIST_HEAD(wait_node_list_write);

struct lock_info* write_accquired[360];
struct list_head read_accquired[360];

/*
	ISSUE
	mutex to spinlock?
	call find_avaialbe in lock?
*/

static void find_available(){
	wait_node *iter;
	int i;

	mutex_lock(rotlock_mutex);

	list_for_each_entry(iter, &wait_node_list_write, list){
		// TODO: check for curret iter
		// for i in range check
		if(true /* able to get lock */){
			// TODO : edit write_acc
			// remove from wait_node_write
			complete(&(iter->comp));
			mutex_unlock(rotlock_mutex);
			return;
		}
	}

	// DO simillar for read

	mutex_unlock(rotlock_mutex);
}

static int lock(int degree, int range, int mode){
	int err;
	// TODO : validate degree, range

	// make waitnode struct
	struct wait_node *mynode = kmalloc(sizeof(wait_node));
	mynode->lockinfo = kmalloc(sizeof(lock_info));
	mynode->lockinfo->degree = degree;
	mynode->lockinfo->range = range;
	mynode->lockinfo->mode = mode;
	mynode->lockinfo->proc = currunt;
	init_completion(&(mynode->comp));

	// accquire lock
	mutex_lock(rotlock_mutex);
	list_add(&(wait_node->list), mode==1?wait_node_list_read.prev:wait_node_list_write.prev);
	mutex_unlock(rotlock_mutex);
// ??? can wrong thread wakeup occurs
	find_available();

	err = wait_for_completion_interruptible(&(mynode->comp));
	if(err == -ERESTARTSYS){
		// TODO : for return erestart, setup restart struct (maybe move wait to another function)
		return -ERESTARTSYS
	} 
	// else err == 0, get lock and return
	return 0;
}

SYSCALL_DEFINE2(rotunlock_read, int, degree, int, range)
{
	// TODO: validate degree, range
	// TODO: check this unlock exist in accuire
	// TODO: if so, remove from acc
	find_available();
	return 0;
}

SYSCALL_DEFINE2(rotunlock_write, int, degree, int, range)
{
	// TODO: validate degree, range
	// TODO: check this unlock exist in accuire
	// TODO: if so, remove from acc
	find_available();
	return 0;
}

SYSCALL_DEFINE1(set_rotation, int, degree)
{
	mutex_lock(rotlock_mutex);
	SYSTEM_DEGREE = degree;
	mutex_unlock(rotlock_mutex);
	find_available();
	return 0;
}

void exit_rotlock(struct task_struct *tsk)
{
	mutex_lock();
	// find all accquired or waiting lock with tsk and remove
	mutex_unlock();
	find_available();
}

// init array of list_head
// call from start_kernel in main.c
void rotlock_init()
{
	int i;
	for(i=0; i<360; i++)
		INIT_LIST_HEAD(read_accquired[i]);
}

SYSCALL_DEFINE2(rotlock_read, int, degree, int, range)
{
	return lock(degree, range, 1);
}

SYSCALL_DEFINE2(rotlock_write, int, degree, int, range)
{
	return lock(degree, range, 2);
}