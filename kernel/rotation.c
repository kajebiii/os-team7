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
static LIST_HEAD(acc_node_list_read);
static LIST_HEAD(acc_node_list_write);

static int write_acc_chk[360];
static int read_acc_chk[360];

/*
	ISSUE
	mutex to spinlock?
	call find_avaialbe in lock?
*/

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

// return total # of wakeup lock
static long find_available(void)
{
	struct lock_info *iter, *temp_node_iter;
	int i, center, range, degree_now;
	int write_in_degree;
	int count_accquired_readlock = 0;

	mutex_lock(&rotlock_mutex);

	write_in_degree = 0;
	list_for_each_entry_safe(iter, temp_node_iter, &wait_node_list_write, list){
		center = iter->degree;
		range = iter->range;
		if(!check_in_range(center, range, SYSTEM_DEGREE)) continue;
		//printk("[rotation] list_for_each %d %d\n", center, range);
		write_in_degree = 1;
		// check for curret iter
		for_each_in_range(center, range, i, degree_now){
			if(write_acc_chk[degree_now] > 0) break;
			if(read_acc_chk[degree_now] > 0) break;
		}
		if(i == range+1){
			// edit write_acc
			for_each_in_range(center, range, i, degree_now)
				write_acc_chk[degree_now]++;
			// remove from wait_node_write
			list_del(&(iter->list));
			list_add(&(iter->list), acc_node_list_write.prev);
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
			if(write_acc_chk[degree_now] > 0) break;
		}
		if(i == range+1){
			// edit read_acc
			for_each_in_range(center, range, i, degree_now){
				read_acc_chk[degree_now]++;
			}
			// remove from wait_node_write
			list_del(&(iter->list));
			list_add(&(iter->list), acc_node_list_read.prev);
			complete(&(iter->comp));
			count_accquired_readlock++;
		}
	}

	mutex_unlock(&rotlock_mutex);
	return count_accquired_readlock;
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
	//printk("[rotation] After wait_for_completion in lock (%d %d %d)\n", degree, range, mode);
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
	mylock->degree = degree;
	mylock->range = range;
	mylock->mode = mode;
	mylock->proc = current;
	init_completion(&(mylock->comp));

	// accquire lock
	mutex_lock(&rotlock_mutex);
	list_add(&(mylock->list), mode==ROTLOCK_MODE_READ?wait_node_list_read.prev:wait_node_list_write.prev);
	mutex_unlock(&rotlock_mutex);
// ??? can wrong thread wakeup occurs
	find_available();
	printk("[rotation] After find_avaliable in lock (%d %d %d)\n", degree, range, mode);
	
	return rotlock_wait(&(mylock->comp));
}

static long unlock(int degree, int range, int mode)
{
	struct lock_info *node;
	int degree_now, i;
	struct list_head* acc_node_list_this_mode = (mode == ROTLOCK_MODE_READ?(&acc_node_list_read):(&acc_node_list_write));

	if(validate_range(degree, range) == 0) return -EINVAL;
	
	mutex_lock(&rotlock_mutex);
	printk("[rotation] After mutex_lock in unlock (%d %d %d)\n", degree, range, mode);

	list_for_each_entry(node, acc_node_list_this_mode, list){
		// check this unlock equals to node
		printk("[rotation] node info!! %d %d %d in unlock\n", node->degree, node->range, node->mode);
		if(node->degree == degree &&
		 node->range == range &&
		 node->proc == current &&
		 node->mode == mode ) 
			break;
		
	}
	printk("[rotation] After list_for in unlock (%d %d %d)\n", degree, range, mode);
	if(&(node->list) == acc_node_list_this_mode){
		printk("[rotation] If inscope in unlock (%d %d %d)\n", degree, range, mode);
		mutex_unlock(&rotlock_mutex);
		return -EINVAL; // return error
	}
	// remove from read_acc
	for_each_in_range(node->degree, node->range, i, degree_now){
		(mode == ROTLOCK_MODE_READ?read_acc_chk:write_acc_chk)[degree_now]--;
	}
	printk("[rotation] After for_each in unlock (%d %d %d)\n", degree, range, mode);
	// remove from wait_node_write
	list_del(&(node->list));
	kfree(node);
	mutex_unlock(&rotlock_mutex);
	printk("[rotation] After mutex_unlock in unlock (%d %d %d)\n", degree, range, mode);

	find_available();
	return 0;
}

void exit_rotlock(struct task_struct *tsk)
{
	struct lock_info *node, *temp_lock_info;
	int i, degree_now;
	int check_deleted = 0;

	mutex_lock(&rotlock_mutex);

	// remove all accquired read lock of task
	list_for_each_entry_safe(node, temp_lock_info, &acc_node_list_read, list){
		// check this unlock equals to node
		if(node->proc == tsk){
			check_deleted = 1;
			for_each_in_range(node->degree, node->range, i, degree_now)
				read_acc_chk[degree_now]--;
			list_del(&(node->list));
			kfree(node);
		}
	}

	// remove all accquired write lock of task
	list_for_each_entry_safe(node, temp_lock_info, &acc_node_list_write, list){
		// check this unlock equals to node
		if(node->proc == tsk){
			check_deleted = 1;
			for_each_in_range(node->degree, node->range, i, degree_now)
				write_acc_chk[degree_now]--;
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

	mutex_unlock(&rotlock_mutex);
	if(check_deleted)
		find_available();
}

SYSCALL_DEFINE1(set_rotation, int, degree)
{
	if(degree < 0 || degree >= 360)
		return -EINVAL;
	printk("[rotation] set_rotation %d\n", degree);
	mutex_lock(&rotlock_mutex);
	SYSTEM_DEGREE = degree;
	mutex_unlock(&rotlock_mutex);
	return find_available();
}

SYSCALL_DEFINE2(rotlock_read, int, degree, int, range)
{
	printk("[rotation] rotlock_read %d\n", degree);
	return lock(degree, range, ROTLOCK_MODE_READ);
}

SYSCALL_DEFINE2(rotlock_write, int, degree, int, range)
{
	printk("[rotation] rotlock_write %d\n", degree);
	return lock(degree, range, ROTLOCK_MODE_WRITE);
}

SYSCALL_DEFINE2(rotunlock_read, int, degree, int, range)
{
	printk("[rotation] rotunlock_read %d\n", degree);
	return unlock(degree, range, ROTLOCK_MODE_READ);
}

SYSCALL_DEFINE2(rotunlock_write, int, degree, int, range)
{
	printk("[rotation] rotunlock_write %d\n", degree);
	return unlock(degree, range, ROTLOCK_MODE_WRITE);
}
