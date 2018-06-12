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
	int M = 1000000, ck = 1, res, i;
	long long u[7];
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
		u[0] = a;
		for (i = 1; i < 6; i++) {
			u[i] = Div(Div(u[i - 1] * a, M)*a, M);
		}
		res = (int)(u[0] - Div(u[1], 6) + Div(u[2], 120) - Div(u[3], 5040) + Div(u[4], 362880));
	}
	return res*ck;
}

long long cosine(long long a) {
	int M = 1000000, ck = 1, res, i;
	long long u[7];
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
		u[0] = M;
		for (i = 1; i <= 6; i++) {
			u[i] = Div(Div(u[i - 1] * a, M)*a, M);
		}
		res = (int)(u[0] - Div(u[1], 2) + Div(u[2], 24) - Div(u[3], 720) + Div(u[4], 40320) + Div(u[5], 3628800));
	}
	return res * ck;
}


long long arccos(long long a) {
	int M = 1000000, i;
	long long u[10];
	if (a > 950000) {
		long long b = 1, e = 1000000, mid, r = 0;
		while (b <= e) {
			mid = (b + e) >> 1;
			if (mid*mid + a*a >= 1ll * M*M) {
				r = mid;
				e = mid - 1;
			}
			else b = mid + 1;
		}
		return r;
	}
	u[0] = a*M;
	for (i = 1; i <= 7; i++) {
		u[i] = Div(Div(u[i - 1] * a, M)*a, M);
	}
	return (int)(3141593 / 2 - Div(u[0] + Div(u[1], 6) + Div(u[2] * 3, 40) + Div(u[3] * 5, 112) + Div(u[4] * 35, 1152) + Div(u[5] * 63, 2816) + Div(u[6] * 63 * 11, 39936), M));
}

long long Mul(long long a, long long b) {

	int ck = 1;
    int M = 100000;
    long long da, db;
	if (a < 0)ck = -ck,a=-a;

	if (b < 0)ck = -ck,b=-b;

	da = Div(a, M), db = Div(b, M);

	return (da * db * M + (a - da*M) * db + da * (b - db*M))*ck;

}

int geo_permission(struct gps_location *loc1, struct gps_location *loc2){
    
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
    long long R = 6400;
    long long L = 20000000;

    int xx1 = x1_int * M + x1_frac;
    int yy1 = y1_int * M + y1_frac;
    int xx2 = x2_int * M + x2_frac;
    int yy2 = y2_int * M + y2_frac;

	long long dx = xx2 - xx1;
	long long dy = yy2 - yy1;
	double dd1;

	long long tx1, tx2, ttt, ty1, tz1, ty2, tz2;
	tx1 = cosine(xx1)*cosine(yy1), ty1 = cosine(xx1)*sine(yy1), tz1 = sine(xx1);
	tx2 = cosine(xx2)*cosine(yy2), ty2 = cosine(xx2)*sine(yy2), tz2 = sine(xx2);
	ttt = Div(Div((Mul(tx1, tx2) + Mul(ty1, ty2) + tz1*tz2*M), M), M);
	if (ttt < 999900) {
		dd1 = Div(arccos(ttt)*R, M);
		dd1 = dd1*dd1;
	}
	else {
		long long t1 = Div(Div(dy * Div(L, M) * cosine((xx1 + xx2) / 2), 180), M);
		long long t2 = Div(Div(L * dx, 180), M);

		dd1 = t1*t1 + t2*t2;
	}

    if(dd1 > acc*acc)return 0;
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
