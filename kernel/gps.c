#include <linux/syscalls.h>
#include <linux/gps.h>
#include <linux/spinlock.h>
#include <linux/namei.h>
#include <linux/slab.h>

struct gps_location current_location = {0, 0, 0, 0, 0};
DEFINE_SPINLOCK(current_location_lock);

int isBetween(int k, int x, int y) {
	return x <= k && k <= y;
}
SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
    int re;
	struct gps_location temp_loc;
    if((re = copy_from_user(&temp_loc, loc, sizeof(struct gps_location))) < 0) return re;
	if(!isBetween(temp_loc.lat_integer,  -90,  +90) || !isBetween(temp_loc.lat_fractional, 0, 999999)) return -EINVAL;
	if(!isBetween(temp_loc.lng_integer, -180, +180) || !isBetween(temp_loc.lng_fractional, 0, 999999)) return -EINVAL;
	if(!(temp_loc.accuracy >= 0)) return -EINVAL;
	spin_lock(&current_location_lock);
	current_location = temp_loc;
	spin_unlock(&current_location_lock);
    return 0;
}

struct inode* get_inode_from_pathname(const char *pathname) {
    struct path path;
    kern_path(pathname, LOOKUP_FOLLOW, &path);
    return path.dentry->d_inode;
}

//On success, the system call should return 0 and *loc should be filled with location information for the specified file.
//This should fail with -EACCES when file not is readable by the current user.
//This should fail with -ENODEV if no GPS coordinates are embedded in the file.
SYSCALL_DEFINE2(get_gps_location, const char __user *, pathname_user, struct gps_location __user *, loc_user) {
	// should be check -EACCES!!!!
	long pathname_len;
	char *pathname;
	struct gps_location loc;
	long err;
	struct inode* inode;
	if (access_ok(VERIFY_WRITE, loc_user, sizeof(struct gps_location)) == 0) return -EINVAL; // Correct?

	pathname_len = strlen_user(pathname_user);
	if(pathname_len <= 0) return -EINVAL; // Correct?

	pathname = kmalloc(pathname_len * sizeof(char), GFP_KERNEL);
	if((err = strncpy_from_user(pathname, pathname_user, pathname_len)) < 0) goto error;
	inode = get_inode_from_pathname(pathname);
	printk("[GETGPS] %d", inode->i_ino);
	if(inode->i_op->get_gps_location) {
		inode->i_op->get_gps_location(inode, &loc);
    	if((err = copy_to_user(loc_user, &loc, sizeof(struct gps_location))) < 0) goto error;
	}else{
		err = -ENODEV;
		goto error;
	}
	kfree(pathname);
	return 0;
error:
	kfree(pathname);
	return err;
}
