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

void Factorization(int n){
    int i;
	for(i=2; i<=n; i++){
		while(n % i == 0){
			n /= i;
			printf("%d", i);
			if(n != 1) printf(" * ");
		}
	}
	puts("");
}

int main(int argc, char *argv[]){
	
	const int BigPrime = 5000011;
	pid_t pid = getpid();
	int i;

	//fork(); fork(); fork(); fork();

	for(i = 1; i <= 20; i++){
		struct timeval starttime, endtime;
		syscall(SYSCALL_SCHED_SETWEIGHT, pid, i);
		sleep(10);

		gettimeofday(&starttime, NULL);
		Factorization(BigPrime);
		gettimeofday(&endtime, NULL);
		
		printf("weight : %d / time : %f s\n", i, (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec) * 0.000001);
	}

}

