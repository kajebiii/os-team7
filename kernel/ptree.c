#include <linux/unistd.h>
#include <linux/kernel.h>

SYSCALL_DEFINE2(ptree, struct prinfo *, buf, int *, nr)
{
	return 0;
}
