#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/slab.h>

#define ROTLOCK_MODE_READ 1
#define ROTLOCK_MODE_WRITE 2

/**
 * for_each_in_range - iterate over a range
 * @center:	center degree of range.
 * @range:	range_value of range.
 * @iter:	variable for save iterator.
 * @value:	variable for contains currunt degree.
 */
#define for_each_in_range(center, range, iter, value) \
	for ((iter)=-(range), (value)=(center)-(range)+(((center)-(range))<0?360:((center)-(range))>=360?-360:0); \
		(iter) <= (range); \
		(iter)++, (value)=(center)+(iter), \
		(value) += ( ((value)<0)?360:0 ), (value) -= (((value)>=360)?360:0) )

static int SYSTEM_DEGREE = 0;

static DEFINE_MUTEX(rotlock_mutex);

struct lock_info{
	int degree, range;
	int mode; // 1 : read, 2 : write
	struct completion comp;
	struct list_head list;
	struct task_struct *proc;
};

static LIST_HEAD(wait_node_list_read);
static LIST_HEAD(wait_node_list_write);
static LIST_HEAD(acq_node_list_read);
static LIST_HEAD(acq_node_list_write);

static int write_acq_chk[360];
static int read_acq_chk[360];

// get distance between x and y in circular case
static inline int get_dist(int x, int y)
{
	if(x<y) {
		int t = y;
		y = x;
		x = t;
	}
	if(x-y <= 180) return x-y;
	else return y-x+360;
}

// return 1 if target in [center-range, center+range] else 0
static inline int check_in_range(int center, int range, int target)
{
	if(get_dist(center, target) <= range) return 1;
	else return 0;
}

static inline int validate_range(int center, int range){
	if(range >= 180 || range <= 0 || center < 0 || center >= 360) return 0;
	return 1;
}

/**
 * static long find_available(void)
 * return total # of wakeup lock
 * must call inside rotlock_mutex
 * unlock mutex before return
 */
static long find_available(void)
{
	struct lock_info *iter, *temp_node_iter;
	int i, center, range, degree_now;
	int write_in_degree;
	int count_acquired_readlock = 0;

	write_in_degree = 0;
	list_for_each_entry_safe(iter, temp_node_iter, &wait_node_list_write, list){
		center = iter->degree;
		range = iter->range;
		if(!check_in_range(center, range, SYSTEM_DEGREE)) continue;
		write_in_degree = 1;
		// check for curret iter
		for_each_in_range(center, range, i, degree_now){
			if(write_acq_chk[degree_now] > 0) break;
			if(read_acq_chk[degree_now] > 0) break;
		}
		if(i == range+1){
			// edit write_acq
			for_each_in_range(center, range, i, degree_now)
				write_acq_chk[degree_now]++;
			// remove from wait_node_write
			list_del(&(iter->list));
			list_add_tail(&(iter->list), &acq_node_list_write);
			complete(&(iter->comp));
			mutex_unlock(&rotlock_mutex);
			return 1;
		}
	}

	if(write_in_degree){
		mutex_unlock(&rotlock_mutex);
		return 0;
	}

	// simillar for read
	list_for_each_entry_safe(iter, temp_node_iter, &wait_node_list_read, list){
		center = iter->degree;
		range = iter->range;
		if(!check_in_range(center, range, SYSTEM_DEGREE)) continue;
		// check for curret iter
		for_each_in_range(center, range, i, degree_now){
			if(write_acq_chk[degree_now] > 0) break;
		}
		if(i == range+1){
			// edit read_acq
			for_each_in_range(center, range, i, degree_now){
				read_acq_chk[degree_now]++;
			}
			// remove from wait_node_write
			list_del(&(iter->list));
			list_add_tail(&(iter->list), &acq_node_list_read);
			complete(&(iter->comp));
			count_acquired_readlock++;
		}
	}

	mutex_unlock(&rotlock_mutex);
	return count_acquired_readlock;
}

static long rotlock_wait_restart(struct restart_block *restart);
static long rotlock_wait(struct completion *comp)
{	
	struct restart_block *restart;
	int err;
	err = wait_for_completion_interruptible(comp);
	if(err == -ERESTARTSYS){
		restart = &current_thread_info()->restart_block;
		restart->fn = rotlock_wait_restart;
		restart->rotlock.comp = comp;
		return -ERESTART_RESTARTBLOCK;
	}
	return 0;
}

static long rotlock_wait_restart(struct restart_block *restart)
{
	return rotlock_wait(restart->rotlock.comp);
}

static long lock(int degree, int range, int mode)
{
	struct lock_info *mylock;

	if(validate_range(degree, range) == 0) return -EINVAL;

	// make waitnode struct
	mylock = kmalloc(sizeof(struct lock_info), GFP_KERNEL);
	if(!mylock) return -ENOMEM;
	mylock->degree = degree;
	mylock->range = range;
	mylock->mode = mode;
	mylock->proc = current;
	init_completion(&(mylock->comp));

	// acquire lock
	mutex_lock(&rotlock_mutex);
	list_add_tail(&(mylock->list), mode==ROTLOCK_MODE_READ?(&wait_node_list_read):(&wait_node_list_write));
	find_available();
	
	return rotlock_wait(&(mylock->comp));
}

static long unlock(int degree, int range, int mode)
{
	struct lock_info *node;
	int degree_now, i;
	struct list_head* acq_node_list_this_mode = (mode == ROTLOCK_MODE_READ?(&acq_node_list_read):(&acq_node_list_write));

	if(validate_range(degree, range) == 0) return -EINVAL;
	
	mutex_lock(&rotlock_mutex);

	list_for_each_entry(node, acq_node_list_this_mode, list){
		// check this unlock equals to node
		if(node->degree == degree &&
		 node->range == range &&
		 node->proc == current &&
		 node->mode == mode ) 
			break;
		
	}
	if(&(node->list) == acq_node_list_this_mode){
		mutex_unlock(&rotlock_mutex);
		return -EINVAL; // return error
	}
	// remove from read_acq
	for_each_in_range(node->degree, node->range, i, degree_now){
		(mode == ROTLOCK_MODE_READ?read_acq_chk:write_acq_chk)[degree_now]--;
	}
	// remove from wait_node_write
	list_del(&(node->list));
	kfree(node);

	find_available();
	return 0;
}

void exit_rotlock(struct task_struct *tsk)
{
	struct lock_info *node, *temp_lock_info;
	int i, degree_now;
	int check_deleted = 0;

	mutex_lock(&rotlock_mutex);

	// remove all acquired read lock of task
	list_for_each_entry_safe(node, temp_lock_info, &acq_node_list_read, list){
		// check this unlock equals to node
		if(node->proc == tsk){
			check_deleted = 1;
			for_each_in_range(node->degree, node->range, i, degree_now)
				read_acq_chk[degree_now]--;
			list_del(&(node->list));
			kfree(node);
		}
	}

	// remove all acquired write lock of task
	list_for_each_entry_safe(node, temp_lock_info, &acq_node_list_write, list){
		// check this unlock equals to node
		if(node->proc == tsk){
			check_deleted = 1;
			for_each_in_range(node->degree, node->range, i, degree_now)
				write_acq_chk[degree_now]--;
			list_del(&(node->list));
			kfree(node);
		}
	}
	
	// remove all waiting read lock of task
	list_for_each_entry_safe(node, temp_lock_info, &wait_node_list_read, list){
		// check this unlock equals to node
		if(node->proc == tsk){
			check_deleted = 1;
			list_del(&(node->list));
			kfree(node);
		}
	}

	// remove all waiting write lock of task
	list_for_each_entry_safe(node, temp_lock_info, &wait_node_list_write, list){
		// check this unlock equals to node
		if(node->proc == tsk){
			check_deleted = 1;
			list_del(&(node->list));
			kfree(node);
		}
	}

	if(check_deleted)
		find_available();
	else mutex_unlock(&rotlock_mutex);
}

SYSCALL_DEFINE1(set_rotation, int, degree)
{
	if(degree < 0 || degree >= 360)
		return -EINVAL;
	mutex_lock(&rotlock_mutex);
	SYSTEM_DEGREE = degree;
	return find_available();
}

SYSCALL_DEFINE2(rotlock_read, int, degree, int, range)
{
	return lock(degree, range, ROTLOCK_MODE_READ);
}

SYSCALL_DEFINE2(rotlock_write, int, degree, int, range)
{
	return lock(degree, range, ROTLOCK_MODE_WRITE);
}

SYSCALL_DEFINE2(rotunlock_read, int, degree, int, range)
{
	return unlock(degree, range, ROTLOCK_MODE_READ);
}

SYSCALL_DEFINE2(rotunlock_write, int, degree, int, range)
{
	return unlock(degree, range, ROTLOCK_MODE_WRITE);
}
