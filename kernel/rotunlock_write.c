#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(rotunlock_write, int, degree, int, range)
{
	return 385;
}
