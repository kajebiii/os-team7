
#include <linux/syscalls.h>
#include <linux/gps.h>
#include <linux/spinlock.h>

struct gps_location current_location = {0, 0, 0, 0, 0};
DEFINE_SPINLOCK(current_location_lock);

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
    int re;
    spin_lock(&current_location_lock);
    re = copy_from_user(&current_location, loc, sizeof(struct gps_location));
    spin_unlock(&current_location_lock);
    return re;
}

SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname, struct gps_location __user *, loc) {
    return -EINVAL;
}
