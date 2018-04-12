#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/list.h>

int SYSTEM_DEGREE = 0;

DEFINE_MUTEX(rotlock_mutex);

struct lock_info{
	struct task_struct *proc;
	int degree, range;
	int mode; // 1 : read, 2 : write
}

struct wait_node{
	struct semaphore sem;
	struct lock_info* lockinfo;
	struct list_head list;
};

static LIST_HEAD(wait_node_list_read);
static LIST_HEAD(wait_node_list_write);

struct lock_info* write_accquired[360];
struct list_head read_accquired[360];

static void find_available(){
	wait_node *iter;

	mutex_lock(rotlock_mutex);

	list_for_each_entry(iter, &wait_node_list_write, list){
		// TODO: check for curret iter

	}


	mutex_unlock(rotlock_mutext);
}

static int lock(int degree, int range, int mode){
	// TODO : validate degree, range

	// make waitnode struct
	struct wait_node *mynode = kmalloc(sizeof(wait_node));
	mynode->lockinfo = kmalloc(sizeof(lock_info));
	mynode->lockinfo->degree = degree;
	mynode->lockinfo->range = range;
	mynode->lockinfo->mode = mode;
	mynode->lockinfo->proc = currunt;
	sema_init(&(mynode->sem), 0);

	// accquire lock
	mutex_lock(rotlock_mutex);
	list_add(&(wait_node->list), mode==1?wait_node_list_read.prev:wait_node_list_write.prev);
	mutex_unlock(rotlock_mutex);

	find_available();
}

SYSCALL_DEFINE2(rotunlock_read, int, degree, int, range)
{
	return 383;
}

SYSCALL_DEFINE2(rotunlock_write, int, degree, int, range)
{
	return 385;
}

SYSCALL_DEFINE1(set_rotation, int, degree)
{
	return 380;
}

void exit_rotlock(struct task_struct *tsk)
{
	//
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