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

void Factorization(int n, int print){
	int i;
	for(i=2; i<=n; i++){
		while(n % i == 0){
			n /= i;
			if(print) {
				printf("%d", i);
				if(n != 1) printf(" * ");
			}
		}
	}
	if(print) puts("");
}

int main(int argc, char *argv[]){

	const int BigPrime = 5000011;
	pid_t pid = getpid();
	int i;

	//fork(); fork(); fork(); fork();
	int p;
	int ITER = 50;

	FILE *out = fopen("output.txt", "w");
	printf("Weight\tTime(s)\n");
	fprintf(out, "Weight\tTime(s)\n");
	for(i = 1; i <= 20; i++){
		double totalTime = 0;
		syscall(SYSCALL_SCHED_SETWEIGHT, pid, i);
		for(p=1; p<=ITER; p++) {
			struct timeval starttime, endtime;

			gettimeofday(&starttime, NULL);
			Factorization(BigPrime, 0);
			gettimeofday(&endtime, NULL);

			//printf("weight : %d / time : %f s\n", i, (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec) * 0.000001);
			totalTime += (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec) * 0.000001;
		}
		printf("%d\t%f s\n", i, totalTime / ITER);
		fprintf(out, "%d\t%f s\n", i, totalTime / ITER);
	}
	return 0;
}

