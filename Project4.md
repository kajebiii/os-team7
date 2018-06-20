# Project 4: Geo-tagged File System

* Assigned: 2017-05-20 Sunday 21:00:00 KST
* Design Review Due: 2017-06-04 Monday KST
* **Due: 2017-06-12 Tuesday 20:59:59 KST**

## Introduction

This project is the last OS project. Yay!

In this project, you will develop a new kernel-level mechanism for embedding location information into ext2 file system metadata and use it for access control.

This is a team project.
Each team will have access to its own GitHub repository (e.g. swsnu/os-team1) for collaboration and submission.
If you have not changed your team, you and your team will continue your work on the same repository you worked on for Project 3.
Your final codes and `README.md` document have to be committed into the _proj4_ branch for submission.
Start from the original [kernel source](https://github.com/swsnu/linux-3.10-artik)(you may already have it in your team repository's _master_ branch) and make incremental changes to get things done.

## 0. Necessary `defconfig` Modification

Open `arch/arm/configs/artik10_defconfig` and add the following line:

```
CONFIG_EXT2_FS=y
```

## 1. Tracking device location (10 pts.)

It would be nice if our devices had GPS sensors to acquire location information, but unfortunately they don't. So we'll implement a system call to update device's current location.

First, write the following definition of `struct gps_location` on `include/linux/gps.h`.
```c
struct gps_location {
  int lat_integer;
  int lat_fractional;
  int lng_integer;
  int lng_fractional;
  int accuracy;
};
```

Write a new system call which updates the kernel with current location of the device, and then write a user space program `gpsupdate` that updates the kernel with the GPS location. Kernel should store location information in kernel memory space.

The new system call number should be 380, and the prototype should be:

```c
int set_gps_location(struct gps_location __user *loc);
```

This sets

* latitude to `loc->lat_integer` + `loc->lat_fractional` * (10^-6) and
* longitude to `loc->lng_integer` + `loc->lng_fractional` * (10^-6)

Fractional parts for latitude and longitude should be nonnegative and do not exceed 999,999. The valid range for latitude is -90 and +90, and longitude is in the range of -180 and +180. `accuracy` should be nonnegative integer and the units of `accuracy` are in meter.

From system boot to the first `sys_gps_location` system call, location is uninitialized. In this case, internal location information may have arbitrary value but it should meet the above criteria.

Please write system call implementation on `kernel/gps.c`. Also, please write source code for `gpsupdate` on `test/gpsupdate.c`. (Or if you have top-level directory `linux-3.10-artik`, on `linux-3.10-artik/test/gpsupdate.c`)

## 2. Add GPS-related operations to inode (20 pts.)

Modify the Linux inode operations interface to include location getter/setter functionality, and then implement the new operations for ext2. For this, you have to modify physical representation of ext2 inode to include location information.

To modify the inode operations interface, add the following two members to the `struct inode_operations` definition in `include/linux/fs.h`:

```c
int (*set_gps_location)(struct inode *);
int (*get_gps_location)(struct inode *, struct gps_location *);
```

Note that the `set_gps_location` function pointer does _not_ have an gps location structure as input - it should use the latest GPS data available in the kernel as set by the `gpsupdate` you wrote for Step 1.

You only need to implement this location-related operations **for the ext2 file system**. Look in the `fs/` directory for all the file system code, and in the `fs/ext2/` directory for ext2 specific code. You will need to change the physical representation of an ext2 inode on disk by appending the following fields _in order_ to the _end_ of the appropriate ext2 inode structure:

```
i_lat_integer (32-bits)
i_lat_fractional (32-bits)
i_lng_integer (32-bits)
i_lng_fractional (32-bits)
i_accuracy (32-bits)
```

These five fields correspond to the `gps_location` structure fields.

You will need to pay close attention to endianness of the fields you add to the ext2 physical inode structure. This data is intended to be read by both big _and_ little endian CPUs.

## 3. Update location information for files (15 pts.)

Modify the file system implementation so that it updates location information of regular files when they are **created** or **modified**. That is, call `set_gps_location` you've implemented whenever a regular file is created or modified.

Being "modified" means that file content have changed. ([Learn more](https://unix.stackexchange.com/questions/2464/timestamp-modification-time-and-created-time-of-a-file)) You don't have to care about directories and symbolic links, but if you want to track location information for them, you're free to do so.

## 4. User-space testing for location information (15 pts.)

You have modified the physical representation of ext2 file system on disk. So you'll also need to modify the user space tool which creates such a file system. You have to use the ext2 file system utilities in [e2fsprogs](http://e2fsprogs.sourceforge.net/). Modify the appropriate file(s) in `e2fsprogs/lib/ext2fs/` to match the new physical layout.

Compile the utilities:

```bash
os@m:~/proj4/e2fsprogs$ ./configure
os@m:~/proj4/e2fsprogs$ make
```

The binary you will be most interested in is `e2fsprogs/misc/mke2fs`. This tool should now create an ext2 file system with your modifications.

Create a modified ext2 file system using the modified `mke2fs` tool _on your Linux PC_.

```bash
os@m:~/proj4$ dd if=/dev/zero of=proj4.fs bs=1M count=1
os@m:~/proj4$ sudo losetup /dev/loop0 proj4.fs
os@m:~/proj4$ sudo ./e2fsprogs/misc/mke2fs -I 256 -L os.proj4 /dev/loop0
os@m:~/proj4$ sudo losetup -d /dev/loop0
```

The file `proj4.fs` should now contain a modified ext2 file system. You can now push the file to your device and mount it:

```bash
os@m:~/proj4$ sdb push proj4.fs /root/proj4.fs
os@m:~/proj4$ sdb shell # or 'shell'
root@artik:/root$ mkdir /root/proj4
root@artik:/root$ mount -o loop -t ext2 /root/proj4.fs /root/proj4
```

You can now create files and directories in `/root/proj4` which should be geo-tagged.

Write a user space utility named `file_loc` which will output a file's embedded location information including the GPS coordinates and a Google Maps link.

In order to retrieve a file's location information, write a new system call numbered 381 with the following prototype:

```c
int get_gps_location(const char __user *pathname, struct gps_location __user *loc);
```

* On success, the system call should return 0 and `*loc` should be filled with location information for the specified file.
* This should fail with `-EACCES` when file not is readable by the current user.
* This should fail with `-ENODEV` if no GPS coordinates are embedded in the file.

Please write system call implementation on `kernel/gps.c`.

The user space utility, `file_loc`, should take exactly one command line argument which is the path to a file. It should then print out the GPS coordinates / accuracy of the specified file and a Google Maps link for the corresponding location. Please write source code for `file_loc` on `test/file_loc.c`. (Or if you have top-level directory `linux-3.10-artik`, on `linux-3.10-artik/test/file_loc.c`) And write a Makefile in `test` directory so that by simply typing `make` we can obtain binaries for `gpsupdate` and `file_loc`.

You need to include the `proj4.fs` file in you final git repository submission, and the file system must contain at least 1 directory and 2 files all of which must have unique GPS coordinates. Show the output of `file_loc` in your `README.md` when called on each of the files / directories you create.

**IMPORTANT**: Make sure to unmount the file system before pulling off the file from the device, otherwise you risk a corruption. If you cannot unmount it, simply reboot the device and pull off the file.

## 5. Location-based file access (15 pts.)

Modify the file system implementation so that files can be only readable from the location where they are created or modified. There are various layers on which implement this mechanism, such as VFS functions, ext2 inode operations, and so on.

You have to consider `accuracy` to allow some errors in location. If the distance between the current device and the file location is shorter than `accuracy`, location-based access control should allow the access. You may want to make some geometrical assumption for the calculation.  Also note that the kernel does not have any floating point or double precision support. Document assumptions or approximations you've used on `README.md`.

This location-based file access does not replace existing access control mechanism, rather it's an extra checklist for ext2 file system. That is, if existing access control mechanism rejects an access, you should not allow it even if file location matches with device location.

You don't have to care about directories and symbolic links, but if you want to implement extra policies for them, you're free to do so. It's advisable to document extra policies on `README.md`.

## Extra problem only for teams of four

This challenge is only for teams with 4 members. A demerit from 5 to 15 points will be marked on project score for not solving this solution properly. Your codes for the extra problem should be commited into _proj4-ext4_ branch for submission.

Repeat from step 2 through 5 for ext4 file system. Please note that

* There may be other ways than modifying inode specification to include location information in file. But storing location information in separate files (not ext4 file system metadata) is not allowed.
* You can just use the system partition in ARTIK10 (mounted on `/`) for testing. (That is, you don't have to use loopback device.)
  * In this case, there are lots of files that has been created before this project is executed. For these files, do not apply location-based file access mechanism. That is, accesses to files without geo-tag should not be rejected because of device location.
  * You may want allow the root user to access files regardless of device location, for sustaining functionality of your ARTIK device. It's advisable to document related policies on `README.md`.

You can use `su` to switch from root user to normal user.
```
$ whoami
root
$ su -l owner
$ whoami
owner
```
