#include <linux/syscalls.h>
#include <linux/gps.h>

struct gps_location curret_location = {0, 0, 0, 0, 0};

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
    return copy_from_user(&current_location, loc, sizeof(struct gps_location));
}
