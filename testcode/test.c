#include <asm/unistd.h>
#include <errno.h>
#include <stdio.h>

int main() {
	int temp = -1;
	int a = 2016, b = 17101;
	temp = syscall(380, &a, &b);

	printf("temp : %d\n", temp);
	
	return 0;
}
