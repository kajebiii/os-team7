#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(rotlock_write, int, degree, int, range)
{
	return 382;
}
