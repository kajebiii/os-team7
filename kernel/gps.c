
#include <linux/syscalls.h>
#include <linux/gps.h>

// TODO: lock will need;

struct gps_location current_location = {0, 0, 0, 0, 0};

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
    return copy_from_user(&current_location, loc, sizeof(struct gps_location));
}

SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname, struct gps_location __user *, loc) {
    return -EINVAL;
}
