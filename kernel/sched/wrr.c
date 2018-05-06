#include <linux/syscalls.h>
#include <linux/sched.h>
#include "sched.h"

void enqueue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
    //
}

void dequeue_task_wrr(struct rq *rq, struct task_struct *p, int flags){
    //
}

void yield_task_wrr (struct rq *rq){
    //
}

bool yield_to_task_wrr (struct rq *rq, struct task_struct *p, bool preempt){
    //
}


const struct sched_class wrr_sched_class = {
    .next = &fair_sched_class,
    .enqueue_task = enqueue_task_wrr,
    .dequeue_task = dequeue_task_wrr,
    .yield_task = yield_task_wrr,
    .yield_to_task = yield_to_task_wrr,

};


/*

	void (*check_preempt_curr) (struct rq *rq, struct task_struct *p, int flags);

	struct task_struct * (*pick_next_task) (struct rq *rq);
	void (*put_prev_task) (struct rq *rq, struct task_struct *p);

#ifdef CONFIG_SMP
	int  (*select_task_rq)(struct task_struct *p, int sd_flag, int flags);
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

	void (*set_curr_task) (struct rq *rq);
	void (*task_tick) (struct rq *rq, struct task_struct *p, int queued);
	void (*task_fork) (struct task_struct *p);

	void (*switched_from) (struct rq *this_rq, struct task_struct *task);
	void (*switched_to) (struct rq *this_rq, struct task_struct *task);
	void (*prio_changed) (struct rq *this_rq, struct task_struct *task,
			     int oldprio);

	unsigned int (*get_rr_interval) (struct rq *rq,
					 struct task_struct *task);

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