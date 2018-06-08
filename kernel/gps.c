#include <linux/syscalls.h>
#include <linux/gps.h>
#include <linux/spinlock.h>
#include <linux/namei.h>
#include <linux/slab.h>

struct gps_location current_location = {0, 0, 0, 0, 0};
DEFINE_SPINLOCK(current_location_lock);

SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
    int re;
    spin_lock(&current_location_lock);
    re = copy_from_user(&current_location, loc, sizeof(struct gps_location));
    spin_unlock(&current_location_lock);
    return re;
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
	long err;
	struct inode* inode;
	if (access_ok(VERIFY_WRITE, loc_user, sizeof(struct gps_location)) == 0) return -EINVAL; // Correct?

	pathname_len = strlen_user(pathname_user);
	if(pathname_len <= 0) return -EINVAL; // Correct?

	pathname = kmalloc(pathname_len * sizeof(char), GFP_KERNEL);
	if((err = strncpy_from_user(pathname, pathname_user, pathname_len)) < 0) goto error;
	inode = get_inode_from_pathname(pathname);
	if(inode->i_op->get_gps_location) {
		inode->i_op->get_gps_location(inode, loc_user);
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
