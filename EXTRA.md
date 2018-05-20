OS Team 7 Project 3 EXTRA

## 1. (Main idea) CPUs collaborate to consist one WRR run queue

In original implementation of WRR run queue, process always moves to tail of run queue when it is migrated by load balancing issue. This can cause moved task have more waiting time than other CPU, and this can be unfair. To handle this problem, we will propose below idea.
Each task will store enqueued time to run queue. When process is migrated to other CPU, this process is not moved to the tail of run queue, but it's moved to position where the run queue is sorted with enqueued time. This means that processes will get CPUs according to enqueued time, even when migrated to other CPU. This idea will give more fairness to task scheduling in WRR.

This idea can be implemented using following method:
	- Enqueued time can be stored in wrr_entity_rq.
	- When task is enqueued to run queue, we change enqueued time to current time.
	- When task is requeued to run queue, we change enqueued time to current time.
	- When task is migrated to other CPU's run queue, we `don't` change enqueued time to current time, and leave it as it is. And we put it in valid position of run queue, which can be found by linear search on migrating CPU's run queue.

## 2. While load balancing, give high priority between cache-sharing CPUs.

Recently CPUs have multiple cache hierachy. In this case, near-core share more caches. So, to prevent many cache misses of task migration in load balancing, give high priority to migrate between cache-share core.

## 3. Considering core speed

In case of ARM big.LITTLE CPU or multiple CPU with different speeds, each core has different performance. So normalize weight_sum to weight_sum/core_speed and do load balance. It can makes each task get fair amount of operation instead just time.

## 4. Using waiting time of each task


## 5. Same process group => Same core

We didn't consider task group in this project. There are task groups in linux. Two different tasks in same task group have lots of same memory with high probability. So, if tasks are in same group, assign them in same core. It will occur high probability cache hit.

