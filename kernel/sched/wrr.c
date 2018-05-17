#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include "sched.h"

void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq) {
	printk("init_wrr_rq visited\n");
	if(wrr_rq == NULL) {
		printk("init wrr_rq is NULL\n");
	}
	else {
		printk("init wrr_rq is NOT NULL\n");
	}
	INIT_LIST_HEAD(&(wrr_rq->run_list));
}

void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
    printk("enqueue_task_wrr visited\n");
	if(p == NULL) printk("WHAT??? enqueue???\n");

	if(!task_current(rq, p)) printk("enqueue_task_wrr: p is already running\n");
	else printk("enqueue_task_wrr: p is not running\n");

	if(!task_current(rq, p)) list_add_tail(&(p->wrr.run_list), &(rq->wrr.run_list));
	
//	inc_nr_running(rq);
}

void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	printk("dequeue_task_wrr visited\n");
	if(p == NULL) printk("WHAT??? dequeue???\n");
	if(!task_current(rq, p)) list_del(&(p->wrr.run_list));	

//	dec_nr_running(rq);
}

void yield_task_wrr (struct rq *rq){
	printk("yield_task_wrr visited\n");
    // yield task
	list_move_tail(&(rq->curr->wrr.run_list), &(rq->wrr.run_list));
	//???
	//list_move_tail(&(current->wrr.run_list), &(rq->wrr.run_list));
}

void check_preempt_curr_wrr (struct rq *rq, struct task_struct *p, int flags){
	printk("check_preempt_curr_wrr visited\n");
	return; 
}

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
	printk("wrr_task_of visited\n");
	/*
#ifdef CONFIG_SCHED_DEBUG
	WARN_ON_ONCE(!rt_entity_is_task(rt_se));
#endif
	*/
	return container_of(wrr_se, struct task_struct, wrr);
}

int mytusvalue = 0;
struct task_struct* pick_next_task_wrr (struct rq *rq){
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct sched_wrr_entity *wrr_entity = list_first_entry_or_null(&(wrr_rq->run_list), struct sched_wrr_entity, run_list);
	
	//printk("pick_next_task_wrr visited\n");
	if(wrr_entity == NULL) {
		// This printk causes Kernel boot failure.
		//if(mytusvalue >= 60000 && mytusvalue % 100 == 0) printk("WRR_ENTITY NULL in pick_next_task_wrr\n");

		mytusvalue++;
		return NULL;
	}
	printk("WRR_ENTITY NOT NULL in pick_next_task_wrr\n");
	wrr_entity->time_slice = wrr_entity->weight * HZ / 100;
	
	list_del(&(wrr_entity->run_list));
	return wrr_task_of(wrr_entity);
	// pick next task to run
}

void put_prev_task_wrr (struct rq *rq, struct task_struct *p){
	printk("put_prev_task_wrr visited\n");
	printk("[WRR SCHEDULER] %x %x %x\n", &(p->wrr.run_list), p->wrr.run_list.next, p->wrr.run_list.prev);
	
	enqueue_task_wrr(rq, p, 0);
	// push task to end
}

#ifdef CONFIG_SMP
int select_task_rq_wrr (struct task_struct *p, int sd_flag, int flags){
	// find cpu of task ??? passive load balance
	// TODO: passive load balance. Look for rt.c

	struct task_struct *curr;
	struct rq *rq;
	int cpu;

	printk("select_task_rq_wrr visited\n");
	cpu = task_cpu(p);

	return cpu;
}
#endif

void set_curr_task_wrr (struct rq *rq){
	printk("set_curr_task_wrr visited\n");
	// something changed
}

void update_curr_wrr(struct rq *rq) {
	// TODO: How to update_curr_wrr?
	//printk("update_curr_wrr visited\n");
}
void task_tick_wrr (struct rq *rq, struct task_struct *p, int queued){
	struct sched_wrr_entity *wrr_se = &p->wrr;

	printk("task_tick_wrr visited\n");

	update_curr_wrr(rq);

	/*
	 * WRR tasks need a special form of timeslice management.
	 * FIFO tasks have no timeslices.
	 */
	if (p->policy != SCHED_WRR)
		return;

	if (--p->wrr.time_slice > 0)
		return;

	printk("Time slice became zero..\n");
	p->wrr.time_slice = p->wrr.weight * HZ / 100;
	printk("Next time slice = %d\n", p->wrr.time_slice);

	/*
	 * Requeue to the end of queue if we (and all of our ancestors) are the
	 * only element on the queue
	 */

	if (wrr_se->run_list.prev != wrr_se->run_list.next) {
		printk("task_tick_wrr: requeue called\n");
		//dequeue_task_wrr(rq, p, 0);
		//enqueue_task_wrr(rq, p, 1);
	    set_tsk_need_resched(p);

		return;
	}
}

void switched_to_wrr (struct rq *this_rq, struct task_struct *task){
	printk("switched_to_wrr visited\n");
	// task's rq changed to wrr
	// Nothing to do here :)
	return;
}

void prio_changed_wrr (struct rq *this_rq, struct task_struct *task, int oldprio){
	printk("prio_change_wrr visited\n");
	// task's prio changed
	// Nothing to do here :)
	return;
}

unsigned int get_rr_interval_wrr (struct rq *rq, struct task_struct *task){
	// round robin??
	// TODO: What??
	struct sched_wrr_entity *wrr = &task->wrr;

	printk("get_rr_interval_wrr visited\n");
	return wrr->time_slice;
}

bool yield_to_task_wrr (struct rq *rq, struct task_struct *p, bool preempt){
	printk("yield_to_task_wrr visited\n");
	return false;
}
#ifdef CONFIG_SMP
void migrate_task_rq_wrr(struct task_struct *p, int next_cpu) {
	printk("migrate_task_rq_wrr visited\n");
}

void pre_schedule_wrr(struct rq *this_rq, struct task_struct *task) {
}
void post_schedule_wrr(struct rq *this_rq) {
}
void task_waking_wrr(struct task_struct *task) {
}
void task_woken_wrr(struct rq *this_rq, struct task_struct *task) {
}
void set_cpus_allowed_wrr(struct task_struct *p, const struct cpumask *newmask) {
}
void rq_online_wrr(struct rq *rq) {
}
void rq_offline_wrr(struct rq *rq) {
}
#endif
void task_fork_wrr (struct task_struct *p) {
}

void switched_from_wrr (struct rq *this_rq, struct task_struct *task) {
}


#ifdef CONFIG_FAIR_GROUP_SCHED
void task_move_group_wrr (struct task_struct *p, int on_rq) {
}
#endif
/*

	bool yield_to_task_wrr (struct rq *rq, struct task_struct *p, bool preempt){

#ifdef CONFIG_SMP
	void (*migrate_task_rq)(struct task_struct *p, int next_cpu);

	void (*pre_schedule) (struct rq *this_rq, struct task_struct *task);
	void (*post_schedule) (struct rq *this_rq);
	void (*task_waking) (struct task_struct *task);
	void (*task_woken) (struct rq *this_rq, struct task_struct *task);

	void (*set_cpus_allowed)(struct task_struct *p,
				 const struct cpumask *newmask);

	void (*rq_online)(struct rq *rq);
	void (*rq_offline)(struct rq *rq);
#endif

	void (*task_fork) (struct task_struct *p);

	void (*switched_from) (struct rq *this_rq, struct task_struct *task);


#ifdef CONFIG_FAIR_GROUP_SCHED
	void (*task_move_group) (struct task_struct *p, int on_rq);
#endif
*/

const struct sched_class wrr_sched_class = {
    .next = &fair_sched_class,
    .enqueue_task = enqueue_task_wrr,
    .dequeue_task = dequeue_task_wrr,
    .yield_task = yield_task_wrr,
    .yield_to_task = yield_to_task_wrr, //NULL,

	.check_preempt_curr = check_preempt_curr,

	.pick_next_task = pick_next_task_wrr,
	.put_prev_task = put_prev_task_wrr,
#ifdef CONFIG_SMP
	.select_task_rq = select_task_rq_wrr,
	.migrate_task_rq = migrate_task_rq_wrr, //NULL,

	.pre_schedule = pre_schedule_wrr, //NULL,
	.post_schedule = post_schedule_wrr, //NULL,
	.task_waking = task_waking_wrr, //NULL,
	.task_woken = task_woken_wrr, //NULL,

	.set_cpus_allowed = set_cpus_allowed_wrr, //NULL,

	.rq_online = rq_online_wrr, //NULL,
	.rq_offline = rq_offline_wrr, //NULL,
#endif
	.set_curr_task = set_curr_task_wrr,
	.task_tick = task_tick_wrr,
	.task_fork = task_fork_wrr, //NULL,
	
	.switched_from = switched_from_wrr, //NULL,
	.switched_to = switched_to_wrr,
	.prio_changed = prio_changed_wrr,

	.get_rr_interval = get_rr_interval_wrr,

#ifdef CONFIG_FAIR_GROUP_SCHED
	.task_move_group = task_move_group_wrr//NULL
#endif
};



SYSCALL_DEFINE2(sched_setweight, pid_t, pid, int, weight){
	return 0;
}

SYSCALL_DEFINE1(sched_getweight, pid_t, pid){
	return 0;
}
