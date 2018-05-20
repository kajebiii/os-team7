#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/list.h>
#include "sched.h"
#include <linux/interrupt.h>

static DEFINE_SPINLOCK(wrr_balancing);
unsigned long wrr_next_balance = 0;

static inline struct task_struct *wrr_task_of(struct sched_wrr_entity *wrr_se)
{
	return container_of(wrr_se, struct task_struct, wrr);
}

static void run_rebalance_domains_wrr(struct softirq_action *h) {
	//printk("Load balance Visited\n");
	int this_cpu = smp_processor_id();

	int min_cpu = this_cpu, max_cpu = this_cpu, cpu;
	struct rq *min_rq = cpu_rq(min_cpu), *max_rq = cpu_rq(max_cpu);
	struct sched_wrr_entity *move_entity;
	struct task_struct *move_task, *max_move_task = NULL;

	unsigned int min_weight_sum, max_weight_sum;
	unsigned int entity_max_weight;

	//if(current_task(rq, p)) return cpu;
	rcu_read_lock();
	for_each_online_cpu(cpu) {
		struct rq *rq = cpu_rq(cpu);
		if(min_rq->wrr.wrr_weight_sum > rq->wrr.wrr_weight_sum) {
			min_cpu = cpu;
			min_rq = rq;
		}
		if(max_rq->wrr.wrr_weight_sum < rq->wrr.wrr_weight_sum) {
			max_cpu = cpu;
			max_rq = rq;
		}
	}
	rcu_read_unlock();

	if(min_rq == max_rq) {
		return;
	}

	local_irq_disable();
	double_rq_lock(min_rq, max_rq);

	min_weight_sum = min_rq->wrr.wrr_weight_sum;
	max_weight_sum = max_rq->wrr.wrr_weight_sum;

	if(min_weight_sum >= max_weight_sum) {
		double_rq_unlock(min_rq, max_rq);
		local_irq_enable();
		return;
	}
	entity_max_weight = (max_weight_sum - min_weight_sum - 1) / 2;
	
	list_for_each_entry(move_entity, &(max_rq->wrr.run_list), run_list) {
		move_task = wrr_task_of(move_entity);
		if(move_entity->weight <= entity_max_weight) {
			if(max_rq->curr != move_task) {
				if(cpumask_test_cpu(min_cpu, tsk_cpus_allowed(move_task))) {
					if(max_move_task == NULL || max_move_task->wrr.weight < move_task->wrr.weight) {
						max_move_task = move_task;
					}
				}
			}
		}
	}

	if(max_move_task == NULL) {
		double_rq_unlock(min_rq, max_rq);
		local_irq_enable();
		return;
	}
	//printk("pid %d move : cpu%d to cpu%d\n", max_move_task->pid, max_cpu, min_cpu);
	deactivate_task(max_rq, max_move_task, 0);
	set_task_cpu(max_move_task, min_cpu);
	activate_task(min_rq, max_move_task, 0);
	check_preempt_curr(min_rq, max_move_task, 0);

	double_rq_unlock(min_rq, max_rq);
	local_irq_enable();
	
	//enum cpu_idle_type idle = this_rq->idle_balance ? CPU_IDLE : CPU_NOT_IDLE;
	//hmp_force_up_migration(this_cpu);
	//rebalance_domains(this_cpu, idle);
	/*
	 * If this cpu has a pending nohz_balance_kick, then do the
	 * balancing on behalf of the other idle cpus whose ticks are
	 * stopped.
	 */
	//nohz_idle_balance(this_cpu, idle);
}
__init void init_sched_wrr_class(void)
{
#ifdef CONFIG_SMP
	open_softirq(SCHED_SOFTIRQ_WRR, run_rebalance_domains_wrr);
#endif
}

/* from fair.c */
static inline int on_null_domain(int cpu)
{
	return !rcu_dereference_sched(cpu_rq(cpu)->sd);
}
void trigger_load_balance_wrr(struct rq *rq, int cpu)
{
	/* Don't need to rebalance while attached to NULL domain */

	spin_lock(&wrr_balancing);
	if (time_after_eq(jiffies, wrr_next_balance) &&
	    likely(!on_null_domain(cpu))) {
		wrr_next_balance = wrr_next_balance + 2 * HZ;
		raise_softirq(SCHED_SOFTIRQ_WRR);
	}
	spin_unlock(&wrr_balancing);
}

void init_wrr_rq(struct wrr_rq *wrr_rq, struct rq *rq) {
	INIT_LIST_HEAD(&(wrr_rq->run_list));
	wrr_next_balance = jiffies;
	wrr_rq->wrr_weight_sum = 0;
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


int mytusvalue = 0;
struct task_struct* pick_next_task_wrr (struct rq *rq){
	struct wrr_rq *wrr_rq = &rq->wrr;
	struct sched_wrr_entity *wrr_entity = list_first_entry_or_null(&(wrr_rq->run_list), struct sched_wrr_entity, run_list);
	if(wrr_entity == NULL) return NULL;
	return wrr_task_of(wrr_entity);
}

void put_prev_task_wrr (struct rq *rq, struct task_struct *p){
}

#ifdef CONFIG_SMP
int select_task_rq_wrr (struct task_struct *p, int sd_flag, int flags){
	//%f\n", wrr_next_balance * 1. / HZ);
	struct rq *rq, *rq2;
	int cpu, cpu2;

	cpu = task_cpu(p);
	
	//if(current_task(rq, p)) return cpu;
	rcu_read_lock();
	for_each_online_cpu(cpu2) {
		rq = cpu_rq(cpu);
		rq2 = cpu_rq(cpu2);
		if(rq->wrr.wrr_weight_sum > rq2->wrr.wrr_weight_sum) {
			if(cpumask_test_cpu(cpu2, tsk_cpus_allowed(p))) {
				cpu = cpu2;
			}
		}
	}
	rcu_read_unlock();
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
	struct sched_wrr_entity *wrr = &task->wrr;
	return wrr->time_slice;
}

#ifdef CONFIG_SMP
void migrate_task_rq_wrr(struct task_struct *p, int next_cpu) {
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


static bool check_same_owner(struct task_struct *p)
{
	const struct cred *cred = current_cred(), *pcred;
	bool match;

	pcred = __task_cred(p);
	match = (uid_eq(cred->euid, pcred->euid) ||
		 uid_eq(cred->euid, pcred->uid));
	return match;
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
	
	ret = -EPERM;
	// check permission
	if(current_uid() != 0){
		if(weight > tsk->wrr.weight) goto end;
		if(!check_same_owner(tsk)) goto end;
	}
	
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
