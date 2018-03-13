#include <asm/unistd.h>
#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <linux/prinfo.h>

int main() {
	struct prinfo *data = NULL;
	int size = 500, re, i;
	int *stk, top=-1;
	while(true){
//		printf("I will malloc !!!\n");		
		data = (struct prinfo*)malloc(size * sizeof(struct prinfo));
		re = syscall(380, data, &size);

//		printf("^^Hi %d %d\n", size, re);
//	for(i=0; i < re && i < size; i++){
//		printf("^^%*s%s,%d,%ld,%d,%d,%d,%d\n", 0, "", data[i].comm, data[i].pid,
//			data[i].state,data[i].parent_pid, data[i].first_child_pid, data[i].next_sibling_pid, data[i].uid);
//	}

//		printf("^^Hello %d %d\n", size, re);
		if(re <= size) break;
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
