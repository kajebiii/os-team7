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
#define SYSCALL_ROTUNLOCK_WRTIE 385

void delay(int sec){
    clock_t start = clock();
    while((clock() - start) / CLOCKS_PER_SEC < sec);
}
int main(int argc, char *argv[]){
	if(argc != 2) {
		printf("Usage ./selector [STARTING INTEGER]");
		return 0;
	}
    int n = atoi(argv[1]);
	if(n <= 1) {
		printf("[STARTING INTEGER] must be large than or equal to 2");
		return 0;
	}
    while(1){
        FILE *fp = fopen("integer", "w");
		//syscall(SYSCALL_ROTLOCK_WRITE, 90, 90);
        fprintf(fp, "%d\n",n);
        printf("selector: %d\n",n);
		fclose(fp);
		//syscall(SYSCALL_ROTUNLOCK_WRITE, 90, 90);
        n++;
        delay(1);
    }
}
