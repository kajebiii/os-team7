#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(rotunlock_read, int, degree, int, range)
{
	return 383;
}
