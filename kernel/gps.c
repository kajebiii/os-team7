#include <linux/syscalls.h>
#include <linux/gps.h>
#include <linux/spinlock.h>
#include <linux/namei.h>
#include <linux/slab.h>

struct gps_location current_location = {0, 0, 0, 0, 0};
DEFINE_SPINLOCK(current_location_lock);

long long cosine(long long a);

long long Div(long long a, long long b) {
	int c = 0;
	long long res = 0;
	int i;
	if (a<0) return -Div(-a, b);
	if (b<0) return -Div(a, -b);
	while (b<a) {
		b <<= 1;
		c++;
	}
	for (i = c; i >= 0; i--) {
		if (a >= b) {
			a -= b;
			res += (1ll << i);
		}
		b >>= 1;
	}
	return res;
}

long long sine(long long a) {
	int M = 1000000, ck = 1, i;
	long long u[7], res;
	if (a < 0) {
		a = -a;
		ck = -1;
	}
	if (a > 90 * M) {
		a = 180 * M - a;
	}
	if (a > 45 * M) {
		res = cosine(90 * M - a);
	}
	else {
		a = Div(a * 3141593, (180 * M));
		u[0] = a*M;
		for (i = 1; i < 6; i++) {
			u[i] = Div(Div(u[i - 1] * a, M)*a, M);
		}
		res = (u[0] - Div(u[1], 6) + Div(u[2], 120) - Div(u[3], 5040) + Div(u[4], 362880));
	}
	return res*ck;
}



long long cosine(long long a) {
	int M = 1000000, ck = 1, i;
	long long u[7], res;
	if (a < 0)a = -a;

	if (a > 90 * M) {
		ck = -1;
		a = 180 * M - a;
	}

	if (a > 45 * M) {
		res = sine(90 * M - a);
	}
	else {
		a = Div(a * 3141593, (180 * M));
		u[0] = 1ll * M*M;
		for (i = 1; i <= 6; i++) {
			u[i] = Div(Div(u[i - 1] * a, M)*a, M);
		}
		res = (u[0] - Div(u[1], 2) + Div(u[2], 24) - Div(u[3], 720) + Div(u[4], 40320) + Div(u[5], 3628800));
	}
	return res * ck;
}

long long Mul(long long a, long long b) {
	int ck = 1, M;
    long long da, db;
	if (a < 0)ck = -ck, a = -a;
	if (b < 0)ck = -ck, b = -b;
	M = 1000000;
	da = Div(a, M), db = Div(b, M);
	return (da * db * M + (a - da*M) * db + da * (b - db*M))*ck;
}


int geo_permission(struct gps_location *loc1, struct gps_location *loc2) {

	int x1_int = loc1->lat_integer;
	int x1_frac = loc1->lat_fractional;
	int y1_int = loc1->lng_integer;
	int y1_frac = loc1->lng_fractional;

	int x2_int = loc2->lat_integer;
	int x2_frac = loc2->lat_fractional;
	int y2_int = loc2->lng_integer;
	int y2_frac = loc2->lng_fractional;

	long long acc = loc1->accuracy;

	int M = 1000000;
	long long R = 6400000;
	long long L = 20000000;

	int xx1 = x1_int * M + x1_frac;
	int yy1 = y1_int * M + y1_frac;
	int xx2 = x2_int * M + x2_frac;
	int yy2 = y2_int * M + y2_frac;


	long long dx = xx2 - xx1;
	long long dy = yy2 - yy1;
	long long dd1;

	long long tx1, tx2, ttt, ty1, tz1, ty2, tz2;
	tx1 = Div(Mul(cosine(xx1), cosine(yy1)), M), ty1 = Div(Mul(cosine(xx1), sine(yy1)), M), tz1 = sine(xx1);
	tx2 = Div(Mul(cosine(xx2), cosine(yy2)), M), ty2 = Div(Mul(cosine(xx2), sine(yy2)), M), tz2 = sine(xx2);
	ttt = Div((Mul(tx1, tx2) + Mul(ty1, ty2) + Mul(tz1, tz2)), M);
	if (ttt > 999999000000ll) {
		long long t1 = Div(Div(dy * Div(L, M) * Div(cosine((xx1 + xx2) / 2), M), 180), M);
		long long t2 = Div(Div(L * dx, 180), M);
		dd1 = t1*t1 + t2*t2;
	}
	else {
		long long ang = Div(Div(acc * 180 * M, 3141593) * M, R);
		long long ttt2;

		if (ang > 180000000)return 1;

		ttt2 = cosine(ang);

		if (ttt2 < ttt)return 1;
		return 0;
	}
	if (dd1 > acc*acc)return 0;
	return 1;

}

inline int isBetween(int k, int x, int y) {
	return x <= k && k <= y;
}
SYSCALL_DEFINE1(set_gps_location, struct gps_location __user *, loc) {
    int re;
	struct gps_location temp_loc;
    if((re = copy_from_user(&temp_loc, loc, sizeof(struct gps_location))) < 0) return re;
	if(!isBetween(temp_loc.lat_integer,  -90,  +90) || !isBetween(temp_loc.lat_fractional, 0, 999999)) return -EINVAL;
	if(!isBetween(temp_loc.lng_integer, -180, +180) || !isBetween(temp_loc.lng_fractional, 0, 999999)) return -EINVAL;
	if(temp_loc.lat_integer == 90 && temp_loc.lat_fractional != 0) return -EINVAL;
	if(temp_loc.lng_integer == 180 && temp_loc.lng_fractional != 0) return -EINVAL;
	if(!(temp_loc.accuracy >= 0)) return -EINVAL;
	spin_lock(&current_location_lock);
	current_location = temp_loc;
	spin_unlock(&current_location_lock);
    return 0;
}

struct inode* get_inode_from_pathname(const char *pathname) {
    struct path path;
    if(kern_path(pathname, LOOKUP_FOLLOW, &path) == 0)
    	return path.dentry->d_inode;
	else
		return NULL;
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
	if (access_ok(VERIFY_WRITE, loc_user, sizeof(struct gps_location)) == 0) return -EFAULT; // Correct?

	pathname_len = strlen_user(pathname_user);
	if(pathname_len <= 0) return -EINVAL; // Correct?

	pathname = kmalloc(pathname_len * sizeof(char), GFP_KERNEL);
	if((err = strncpy_from_user(pathname, pathname_user, pathname_len)) < 0) goto error;
	inode = get_inode_from_pathname(pathname);
	if(inode == NULL) {
		err = -ENOENT;
		goto error;
	}
	if(generic_permission(inode, MAY_READ) != 0) {
		err = -EACCES;
		goto error;
	}
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
