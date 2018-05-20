OS Team 7 Project 3 EXTRA

## 1. (Main idea) CPUs collaborate to consist one WRR run queue

In original implementation of WRR run queue, process always moves to tail of run queue when it is migrated by load balancing issue. This can cause moved task have more waiting time than other CPU, and be unfair. To handle this problem, we thought about CPUs. 

## 2. While load balancing, give high priority between cache-sharing CPUs.


## 3. Considering core speed


## 4. Using waiting time of each task


## 5. Same process group => Same core

