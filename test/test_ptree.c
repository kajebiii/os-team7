#include <asm/unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

struct prinfo {
  long state;             /* current state of process */
  pid_t pid;              /* process id */
  pid_t parent_pid;       /* process id of parent */
  pid_t first_child_pid;  /* pid of oldest child */
  pid_t next_sibling_pid; /* pid of younger sibling */
  long uid;               /* user id of process owner */
  char comm[64];          /* name of program executed */
};


int main() {
	struct prinfo *data = NULL;
	int size = 500, re, i;
	int *stk, top=-1;
	while(1){		
		data = (struct prinfo*)malloc(size * sizeof(struct prinfo));
		re = syscall(380, data, &size);
		if(re == size) break;
		size = re + 10;
		free(data);
	}
	stk = malloc((re + 10) * sizeof(int));
	for(i=0; i<re; i++){
		while(top != -1 && stk[top] != data[i].parent_pid) top--;
		stk[++top] = data[i].pid;
		printf("%*s%s,%d,%ld,%d,%d,%d,%d\n", top*2, "", data[i].comm, data[i].pid,
			data[i].state,data[i].parent_pid, data[i].first_child_pid, data[i].next_sibling_pid, data[i].uid);
	}
	free(data);
	free(stk);
	return 0;
}
