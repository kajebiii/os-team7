#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include "sched.h"


void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq) {
	INIT_LIST_HEAD(&(wrr_rq->run_list));
}

void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	p->wrr.wrr_rq = rq;
	list_add_tail(&(p->wrr.run_list), &(rq->wrr.run_list));
	inc_nr_running(rq);
}

void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	p->wrr.wrr_rq = NULL;
	if(!list_empty(&(p->wrr.run_list))) {
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
	// TODO: passive load balance. Look for rt.c
	struct task_struct *curr;
	struct rq *rq;
	int cpu;
	cpu = task_cpu(p);
	return cpu;
}
#endif

void set_curr_task_wrr (struct rq *rq){
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
	return;
}

void prio_changed_wrr (struct rq *this_rq, struct task_struct *task, int oldprio){
	return;
}

unsigned int get_rr_interval_wrr (struct rq *rq, struct task_struct *task){
	// round robin??
	// TODO: What??
	struct sched_wrr_entity *wrr = &task->wrr;
	return wrr->time_slice;
}

bool yield_to_task_wrr (struct rq *rq, struct task_struct *p, bool preempt){
	return false;
}
#ifdef CONFIG_SMP
void migrate_task_rq_wrr(struct task_struct *p, int next_cpu) {
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
	.task_move_group = task_move_group_wrr//NULL
#endif
};


SYSCALL_DEFINE2(sched_setweight, pid_t, pid, int, weight)
{
	return 0;
}
SYSCALL_DEFINE1(sched_getweight, pid_t, pid)
{
	return 0;
}
