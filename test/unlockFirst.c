#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define SYSCALL_SET_ROTATION 380
#define SYSCALL_ROTLOCK_READ 381
#define SYSCALL_ROTLOCK_WRITE 382
#define SYSCALL_ROTUNLOCK_READ 383
#define SYSCALL_ROTUNLOCK_WRITE 385


int main(int argc, char *argv[]){
	if(argc != 2) {
		printf("Usage ./selector [INTEGER IDENTIFIER]");
		return 0;
	}
    int id = atoi(argv[1]);
	if(id <= -1) {
		printf("[INTEGER IDENTIFIER] must be large than or equal to 0");
		return 0;
	}
	printf("W : %ld\n", syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90));
	printf("RL : %ld\n", syscall(SYSCALL_ROTLOCK_READ, 90, 1));
	printf("R : %ld\n", syscall(SYSCALL_ROTUNLOCK_READ, 90, 90));
	printf("RL : %ld\n", syscall(SYSCALL_ROTLOCK_READ, 90, 2));
	printf("R : %ld\n", syscall(SYSCALL_ROTUNLOCK_READ, 90, 90));
	printf("RL : %ld\n", syscall(SYSCALL_ROTLOCK_READ, 90, 3));
	printf("R : %ld\n", syscall(SYSCALL_ROTUNLOCK_READ, 90, 90));
	printf("RL : %ld\n", syscall(SYSCALL_ROTLOCK_READ, 90, 4));
	printf("R : %ld\n", syscall(SYSCALL_ROTUNLOCK_READ, 90, 90));
	printf("W : %ld\n", syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90));
	printf("W : %ld\n", syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90));
	printf("W : %ld\n", syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90));
	printf("W : %ld\n", syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90));
	printf("W : %ld\n", syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90));
	while(1) sleep(1);
	return 0;
}
