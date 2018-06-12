# OS Team 7 Project 4 README

## How to build kernel
The way to build kernel does not differ from original kernel. Type `./build` in the root folder, and kernel will be built. Or you can type `make build` in the "test" folder.

## Flashing the Device
The way to flash the device is the same as original.

After the building is done.

1. Open the debug console. (Use `screen` command or PuTTY)
1. Hit reset button on the device.
1. Push and hold power button for 1 second.
1. You'll see `Hit any key to stop autoboot`. Press any key.
1. Type `thordown` and <kbd>Enter</kbd>. The device will enter thordown mode.

Just type `lthor image.tar` in the root folder, or `make lthor` in the "test" folder.

## How to execute test code
```bash
HOST$ XXX
```
* It means "type XXX at the terminal of your Ubuntu-PC."

```bash
BOARD$ YYY
```
* It means "Type YYY at the terminal of Artik 10."

After logging in, you have to execute 
```bash
BOARD$ direct_set_debug.sh --sdb-set
```
to enable sdb. 


In the "test" folder, type 
```bash
HOST$ make clean
HOST$ make
HOST$ make run
```
Then, It will compile test codes (includes file_loc.c, gpsupdate.c) and transfer our test excutable files (includes file_loc, gpsupdate) between the device and your PC.
Also, it will transfer proj4.fs. pro4.fs have one directory(named "directory") and two files(named "snu301" and "snu25").

The following is output of `file_loc` when called on our files and directory.
```
BOARD$ mkdir /root/proj4
BOARD$ cd /root/
BOARD$ mount -o loop -t ext2 /root/proj4.fs /root/proj4
BOARD$ cd /root/proj4
BOARD$ ../file_loc snu301
latitude:       37.450005
longitude:      126.952366
accuracy:       50(m)

Google Map Link
https://www.google.com/maps/@37.450005,126.952366,15z
BOARD$ ../file_loc snu25
latitude:       37.458773
longitude:      126.949533
accuracy:       50(m)

Google Map Link
https://www.google.com/maps/@37.458773,126.949533,15z
BOARD$ ../file_loc directory
File_Loc fail!
Error 19, No such device
```

And if you call `gpsupdate` you can change your location. The following is example.
```
BOARD$ /root/gpsupdate
Enter latitude integer!
39
Enter latitude fractional!
123145
Enter longitude integer!
130
Enter longitude fractional!
977235
Enter accuracy!
731
Update success!
BOARD$ /root/gpsupdate
Enter latitude integer!
89
Enter latitude fractional!
123123
Enter longitude integer!
180
Enter longitude fractional!
1
Enter accuracy!
1
GpsUpdate fail!
Error 22, Invalid argument
```

After testing, recommmend to execute following command for unmount
```
BOARD$ umount -dl /root/proj4
```

You may need to open two or many terminal.


## [Comparing changes](https://github.com/swsnu/os-team7/compare/base...proj4)
* The above comparing changes show 1800+ files change. (Because of adding e2fsprogs files)
* The below link show comparing changes without e2fsprogs files
* [Comparing changes (Before adding e2fsprogs)](https://github.com/swsnu/os-team7/compare/base...c6ee4f87bfc074ae929cd2f4f22eb5d619cee2e3)
* [Comparing changes (After adding e2fsprogs)](https://github.com/swsnu/os-team7/compare/cb0c31037407569a7c035be29ecdcf3b46911e7b...proj4)

## Preprocess
* Add new system call
	* [arch/arm/include/asm/unistd.h](https://github.com/swsnu/os-team7/blob/proj4/arch/arm/include/asm/unistd.h)
	* [arch/arm/include/uapi/asm/unistd.h](https://github.com/swsnu/os-team7/blob/proj4/arch/arm/include/uapi/asm/unistd.h)
	* [arch/arm/kernel/calls.S](https://github.com/swsnu/os-team7/blob/proj4/arch/arm/kernel/calls.S)

* Add new inode operation
	* [fs/ext2/ext2.h](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/ext2.h)
	* [fs/ext2/file.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/file.c)
	* [include/linux/fs.h](https://github.com/swsnu/os-team7/blob/proj4/include/linux/fs.h)

* Add new struct/variables/functions for GPS location information
	* [e2fsprogs/lib/ext2fs/ext2_fs.h](https://github.com/swsnu/os-team7/blob/proj4/e2fsprogs/lib/ext2fs/ext2_fs.h)
	* [fs/ext2/ext2.h](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/ext2.h)
	* [fs/ext2/inode.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/inode.c)
	* [include/linux/gps.h](https://github.com/swsnu/os-team7/blob/proj4/include/linux/gps.h)
		
## Main implementation 
* [kernel/gps.c](https://github.com/swsnu/os-team7/blob/proj4/kernel/gps.c)
	* current_location: Global variable contains system's current location
	* set_gps_location syscall
		* Get user's location input and check whether it's valid.
		* Change current_location global variable.
		* return 0 on success, -EINVAL on invalid location input.
	* get_gps_location syscall
		* Get user's pathname and check whether it's valid.
		* Check whether pathname points to valid file.
		* Check user's permission to file.
		* Get file's current location
		* Write file's current location to user pointer, and returns.
		* return 0 on success, -ENOENT on invalid filepath, -EFAULT on invalid user pointer, -EINVAL on invalid path name, -EACCES on no permission, -ENODEV on invalid gps position.
	* geo_permission
		* Calculates distance between file's current location and device's current location, and check whether it's valid.
		* If two location can be same, returns 1. else returns 0.
		* Methods used to calculate distance are described below section.

* [fs/ext2/inode.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/inode.c)
	* ext2_set_gps_location
	 	* Set inode's location to current location.
	* ext2_get_gps_location
		* Get inode's location value to designated pointer.
	* ext2_permission
		* Calls geo_permission function to check whether location is valid or not.
		* If geo_permission fails, returns -EACCES. Else, calls generic_permission and returns.

## Other modified files to set gps location
* [fs/attr.c](https://github.com/swsnu/os-team7/blob/proj4/fs/attr.c)
* [fs/binfmt_misc.c](https://github.com/swsnu/os-team7/blob/proj4/fs/binfmt_misc.c)
* [fs/buffer.c](https://github.com/swsnu/os-team7/blob/proj4/fs/buffer.c)
* [fs/ext2/dir.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/dir.c)
* [fs/ext2/ialloc.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/ialloc.c)
* [fs/ext2/inode.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/inode.c)
* [fs/ext2/super.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/super.c)
* [fs/libfs.c](https://github.com/swsnu/os-team7/blob/proj4/fs/libfs.c)
* [fs/pipe.c](https://github.com/swsnu/os-team7/blob/proj4/fs/pipe.c)
* [fs/splice.c](https://github.com/swsnu/os-team7/blob/proj4/fs/splice.c)
* [kernel/gps.c](https://github.com/swsnu/os-team7/blob/proj4/kernel/gps.c)
* [mm/filemap.c](https://github.com/swsnu/os-team7/blob/proj4/mm/filemap.c)
* [mm/filemap_xip.c](https://github.com/swsnu/os-team7/blob/proj4/mm/filemap_xip.c)
* [mm/memory.c](https://github.com/swsnu/os-team7/blob/proj4/mm/memory.c)

## How to calculate distance between two points
* Assumption
	* Earth is a perfect sphere
	* Earth's perimeter is 40,000km = 4 * 10^7 m
* long long Div(long long a, long long b)
	* In the kernel, we can't use divide operation(/) between two long long values.
	* Use basic opertiaons(<<, >>, +, -) only
	* When a < 0, return -Div(-a, b) / When b < 0, return -Div(a, -b)
	* Find minimum integer c that satisfy b * 2^c >= a
	* Use division algorithm in binary system (learned in elementary school)
* int cosine(long long a)
	* ![cos](https://wikimedia.org/api/rest_v1/media/math/render/svg/b76af64626b80d6f66bdb964e1794c373b611479)
	* Get value using taylor expansion method
	* Caculate first five terms
	* return cos(a / 1000000) value
* int sine(long long a)
	* ![sine](https://wikimedia.org/api/rest_v1/media/math/render/svg/def345e147219a7892eb8140dfeb1c77b29dce38)
	* Get value using taylor expansion method
	* Caculate first five terms
	* return sin(a / 1000000) value

## EXTRA: ext4
* [Our submission branch (ext4)](https://github.com/swsnu/os-team7/tree/proj4-ext4)
* ext4 is quite similar to ext2, but following are different from ext2:
	* Location information is not stored as variable in struct inode, but it's stored using xattr.
	* Because ext4 file system is used mainly in booting kernel/etc, these programs should be executed without location check procedure.
		* When there is no location information set, skip location check procedure and continues general permission check.
	* Test could be done without loopback device on ext4 file system, so we did not use it here.

### Main implementation (ext4)
* [kernel/gps.c](https://github.com/swsnu/os-team7/blob/proj4-ext4/fs/ext4/inode.c)
	* ext4_set_gps_location
		* set gps_location value using ext4_xattr_set function
	* ext4_get_gps_location
		* get gps_location value using ext4_xattr_get function
		* when there is no gps_location in file ... TODO TODO TODO
	* ext4_permission
		* check inode's access permission
		* if user is root, just returns generic_permission result.
		* else if file's position is never set, location informations are not checked and generic_permission result is returned.
		* else checks location information (calls geo_permission), and returns error or calls generic_permission.
### Other modified files to set gps location (ext4)
* [kernel/gps.c](https://github.com/swsnu/os-team7/blob/proj4-ext4/fs/ext4/inode.c)
	* ext4_page_mkwrite
		* kernel funciton
		* add set_gps_location(inode) after update_time function called

## Any lessons learned
* We learned about ext2 file system and structure of inode. We had to add new attributes to inode, and implement necessary functions, structs and global variables to implement it.
* We learned about checking file permission. User with no permission should not access to file, and we can check it using kernel functions.
* We learned how to calculate distance on sphere without using floating point operations and long long division. We had to implement distance calculation on sphere to check file permission, 
* We learned how to use xattr in ext4 to handle extra attributes. We used these features to implement geographical location in ext4 file system.

