#include <linux/syscalls.h>
#include <linux/sched.h>
#include "sched.h"

void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
    // add task to runqueue
}

void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
	struct sched_wrr_entity *wrr = &p->wrr;
	list_del(&(wrr->run_list));
}

void yield_task_wrr (struct rq *rq){
    // yield task
}

void check_preempt_curr_wrr (struct rq *rq, struct task_struct *p, int flags){
	// 
}

struct task_struct* pick_next_task_wrr (struct rq *rq){
	// pick next task to run
}

void put_prev_task_wrr (struct rq *rq, struct task_struct *p){
	// push task to end
}

int select_task_rq_wrr (struct task_struct *p, int sd_flag, int flags){
	// find cpu of task ??? passive load balance
}

void set_curr_task_wrr (struct rq *rq){
	// something changed
}

void task_tick_wrr (struct rq *rq, struct task_struct *p, int queued){
	// call each task tick
}

void switched_to_wrr (struct rq *this_rq, struct task_struct *task){
	// task's rq changed to wrr
}

void prio_changed_wrr (struct rq *this_rq, struct task_struct *task, int oldprio){
	// task's prio changed
}

unsigned int get_rr_interval_wrr (struct rq *rq, struct task_struct *task){
	// round robin??
}


const struct sched_class wrr_sched_class = {
    .next = &fair_sched_class,
    .enqueue_task = enqueue_task_wrr,
    .dequeue_task = dequeue_task_wrr,
    .yield_task = yield_task_wrr,
    .yield_to_task = NULL,

	.check_preempt_curr = check_preempt_curr,

	.pick_next_task = pick_next_task_wrr,
	.put_prev_task = put_prev_task_wrr,
#ifdef CONFIG_SMP
	.select_task_rq = select_task_rq_wrr,
	.migrate_task_rq = NULL,

	.pre_schedule = NULL,
	.post_schedule = NULL,
	.task_waking = NULL,
	.task_woken = NULL,

	.set_cpus_allowed = NULL,

	.rq_online = NULL,
	.rq_offline = NULL,
#endif
	.set_curr_task = set_curr_task_wrr,
	.task_tick = task_tick_wrr,
	.task_fork = NULL,
	
	.switched_from = NULL,
	.switched_to = switched_to_wrr,
	.prio_changed = prio_changed_wrr,

	.get_rr_interval = get_rr_interval_wrr,

#ifdef CONFIG_FAIR_GROUP_SCHED
	.task_move_group = NULL
#endif
};


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

SYSCALL_DEFINE2(sched_setweight, pid_t, pid, int, weight){
	return 0;
}

SYSCALL_DEFINE1(sched_getweight, pid_t, pid){
	return 0;
}
