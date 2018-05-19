#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include "sched.h"


void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq) {
	INIT_LIST_HEAD(&(wrr_rq->run_list));
}

void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	rq->wrr.wrr_weight_sum += p->wrr.weight;
	list_add_tail(&(p->wrr.run_list), &(rq->wrr.run_list));
	inc_nr_running(rq);
}

void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	if(!list_empty(&(p->wrr.run_list))) {
		rq->wrr.wrr_weight_sum -= p->wrr.weight;
		list_del(&(p->wrr.run_list));
		INIT_LIST_HEAD(&(p->wrr.run_list));
		dec_nr_running(rq);
	}
}

void yield_task_wrr (struct rq *rq){
	list_move_tail(&(rq->curr->wrr.run_list), &(rq->wrr.run_list));
}

void check_preempt_curr_wrr (struct rq *rq, struct task_struct *p, int flags){
	return; 
}

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
	return container_of(wrr_se, struct task_struct, wrr);
}

int mytusvalue = 0;
struct task_struct* pick_next_task_wrr (struct rq *rq){
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct sched_wrr_entity *wrr_entity = list_first_entry_or_null(&(wrr_rq->run_list), struct sched_wrr_entity, run_list);
	if(wrr_entity == NULL) return NULL;
	wrr_entity->time_slice = wrr_entity->weight * HZ / 100;
	return wrr_task_of(wrr_entity);
}

void put_prev_task_wrr (struct rq *rq, struct task_struct *p){
}

#ifdef CONFIG_SMP
int select_task_rq_wrr (struct task_struct *p, int sd_flag, int flags){
	// find cpu of task ??? passive load balance
	// TODO: passive load balance. Look for rt.c (Done?)

	struct task_struct *curr;
	struct rq *rq, *rq2;
	int cpu, cpu2;

	cpu = task_cpu(p);
	
	//if(current_task(rq, p)) return cpu;
	rcu_read_lock();
	for_each_online_cpu(cpu2) {
		rq = cpu_rq(cpu);
		rq2 = cpu_rq(cpu2);
		if(rq->wrr.wrr_weight_sum > rq2->wrr.wrr_weight_sum) cpu = cpu2;
	}
	rcu_read_unlock();
	return cpu;
}
#endif

void set_curr_task_wrr (struct rq *rq){
//	struct task_struct *p = rq->curr;
//	p->se.exec_start = rq->clock_task;
}

void update_curr_wrr(struct rq *rq) {
}
void task_tick_wrr (struct rq *rq, struct task_struct *p, int queued){
	struct sched_wrr_entity *wrr_se = &p->wrr;
	update_curr_wrr(rq);
	if (p->policy != SCHED_WRR)
		return;

	if (--p->wrr.time_slice > 0)
		return;

	p->wrr.time_slice = p->wrr.weight * HZ / 100;
	if (wrr_se->run_list.prev != wrr_se->run_list.next) {
		dequeue_task_wrr(rq, p, 0);
		enqueue_task_wrr(rq, p, 1);
	    set_tsk_need_resched(p);
		return;
	}
}

void switched_to_wrr (struct rq *this_rq, struct task_struct *task){
	// Nothing to do here
	return;
}

void prio_changed_wrr (struct rq *this_rq, struct task_struct *task, int oldprio){
	// Nothing to do here
	return;
}

unsigned int get_rr_interval_wrr (struct rq *rq, struct task_struct *task){
	// round robin??
	struct sched_wrr_entity *wrr = &task->wrr;
	return wrr->time_slice;
}

#ifdef CONFIG_SMP
void migrate_task_rq_wrr(struct task_struct *p, int next_cpu) {
	// (maybe) Nothing to do here
}

#endif

const struct sched_class wrr_sched_class = {
    .next = &fair_sched_class,
    .enqueue_task = enqueue_task_wrr,
    .dequeue_task = dequeue_task_wrr,
    .yield_task = yield_task_wrr,
    //.yield_to_task = yield_to_task_wrr, //NULL,

	.check_preempt_curr = check_preempt_curr_wrr,

	.pick_next_task = pick_next_task_wrr,
	.put_prev_task = put_prev_task_wrr,
#ifdef CONFIG_SMP
	.select_task_rq = select_task_rq_wrr,
	.migrate_task_rq = migrate_task_rq_wrr,

	//.pre_schedule = pre_schedule_wrr, //NULL,
	//.post_schedule = post_schedule_wrr, //NULL,
	//.task_waking = task_waking_wrr, //NULL,
	//.task_woken = task_woken_wrr, //NULL,

	//.set_cpus_allowed = set_cpus_allowed_wrr, //NULL,

	//.rq_online = rq_online_wrr, //NULL,
	//.rq_offline = rq_offline_wrr, //NULL,
#endif
	.set_curr_task = set_curr_task_wrr,
	.task_tick = task_tick_wrr,
	//.task_fork = task_fork_wrr, //NULL,
	
	//.switched_from = switched_from_wrr, //NULL,
	.switched_to = switched_to_wrr,
	.prio_changed = prio_changed_wrr,

	.get_rr_interval = get_rr_interval_wrr,

#ifdef CONFIG_FAIR_GROUP_SCHED
	//	.task_move_group = task_move_group_wrr//NULL
#endif
};

/*
 * task_rq_lock - lock p->pi_lock and lock the rq @p resides on.
 */
static struct rq *task_rq_lock(struct task_struct *p, unsigned long *flags)
	__acquires(p->pi_lock)
	__acquires(rq->lock)
{
	struct rq *rq;

	for (;;) {
		raw_spin_lock_irqsave(&p->pi_lock, *flags);
		rq = task_rq(p);
		raw_spin_lock(&rq->lock);
		if (likely(rq == task_rq(p)))
			return rq;
		raw_spin_unlock(&rq->lock);
		raw_spin_unlock_irqrestore(&p->pi_lock, *flags);
	}
}

static inline void
task_rq_unlock(struct rq *rq, struct task_struct *p, unsigned long *flags)
	__releases(rq->lock)
	__releases(p->pi_lock)
{
	raw_spin_unlock(&rq->lock);
	raw_spin_unlock_irqrestore(&p->pi_lock, *flags);
}

SYSCALL_DEFINE2(sched_setweight, pid_t, pid, int, weight)
{
	struct task_struct *tsk = NULL;
	int ret = -EINVAL, old_weight;
	unsigned long flags;
	struct rq *rq;

	if(pid < 0) return -EINVAL;
	if(weight <= 0 || weight > 20) return -EINVAL;

	rcu_read_lock();
	tsk = pid ? find_task_by_vpid(pid) : current;
	if(tsk == NULL){
		rcu_read_unlock();
		return -EINVAL;
	}

	rq = task_rq_lock(tsk, &flags);
	if(tsk->policy != SCHED_WRR) goto end;
	
	// check permission
	
	old_weight = tsk->wrr.weight;
	tsk->wrr.weight = weight;

	rq->wrr.wrr_weight_sum += weight - old_weight; // update weight sum

	if(rq->curr != tsk) { 		// update timeslice
		tsk->wrr.time_slice = weight * HZ / 100;
	}
	ret = 0;

end:
	task_rq_unlock(rq, tsk, &flags);
	rcu_read_unlock();
	return ret;
}

SYSCALL_DEFINE1(sched_getweight, pid_t, pid)
{
	struct task_struct *tsk = NULL;
	int ret = -EINVAL;
	if(pid < 0) return -EINVAL;

	rcu_read_lock();
	tsk = pid ? find_task_by_vpid(pid) : current;
	if(tsk == NULL) goto end;
	if(tsk->policy != SCHED_WRR) goto end;
	ret = tsk->wrr.weight;	
end:
	rcu_read_unlock();
	return ret;
}
