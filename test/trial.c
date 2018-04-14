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
		printf("Usage ./selector [INTEGER IDENTIFIER]");
		return 0;
	}
    int id = atoi(argv[1]);
	if(id <= -1) {
		printf("[INTEGER IDENTIFIER] must be large than or equal to 0");
		return 0;
	}
    while(1){
        int n, i;
        FILE *fp = fopen("integer", "r");
		//syscall(SYSCALL_ROTLOCK_READ, 90, 90);
        fscanf(fp, "%d", &n);
		printf("trial-%d: %d = ", id, n);
        for(i=2; i<=n; i++){
            while(n % i == 0){
                n /= i;
                printf("%d", i);
                if(n != 1) printf(" * ");
            }
        }
		puts("");
		fclose(fp);
		//syscall(SYSCALL_ROTLOCK_READ, 90, 90);
        delay(1);
    }
}
