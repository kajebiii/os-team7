#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
	int degree = 2, range = 1;
	printf("%ld\n", syscall(380, degree));
	printf("%ld\n", syscall(381, degree, range));
	printf("%ld\n", syscall(382, degree, range));
	printf("%ld\n", syscall(383, degree, range));
	printf("%ld\n", syscall(385, degree, range));
	return 0;
}
