#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

char *strerror(int errnum);

int main(){
    int n, i;

    printf("My Policy Before %d\n", sched_getscheduler(0));
    struct sched_param param;
    param.sched_priority = 0;

    printf("return value = %d\n", sched_setscheduler(0, 0, &param));
	fork();
	fork();
	fork();
	fork();
	fork();
	while(1);
    return 0;
}
