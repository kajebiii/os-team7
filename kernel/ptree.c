#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/prinfo.h>

SYSCALL_DEFINE2(ptree, prinfo *, buf, int *, nr)
{
	return 0;
}
