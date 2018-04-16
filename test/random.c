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
    while(1){
		int a = rand()%360, b = rand()%179 + 1;
		syscall(SYSCALL_ROTLOCK_READ, a, b);
		printf("[%d %d]\n", a, b);
		syscall(SYSCALL_ROTUNLOCK_READ, a, b);
        sleep(1);
    }
}
