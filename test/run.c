#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

char *strerror(int errnum);

int main(){
    int n, i;

    printf("My Policy Before %d\n", sched_getscheduler(0));
    fork();
    fork();

    struct sched_param param;
    param.sched_priority = 50;

    sched_setscheduler(0, 6, &param);
    printf("%s\n", strerror(errno));
    printf("My Policy After %d\n", sched_getscheduler(0));

    fork();
    fork();

    for(i=1; i<200000000 ; i++){
        if(i == 100000000) printf("%d\n", i);
    }
    return 0;
}