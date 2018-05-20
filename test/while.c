#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define SYSCALL_SCHED_SETWEIGHT 380
#define SYSCALL_SCHED_GETWEIGHT 381


int main(int argc, char *argv[]) {
	srand(time(NULL));
	syscall(SYSCALL_SCHED_SETWEIGHT, 0, rand() % 20 + 1);
	while(1);
	return 0;
}

