#include <linux/syscalls.h>


SYSCALL_DEFINE2(sched_setweight, pid_t, pid, int, weight){
	return 0;
}

SYSCALL_DEFINE1(sched_getweight, pid_t, pid){
	return 0;
}