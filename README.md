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
Then, It will compile test codes and transfer our test excutable files between the device and your PC.

TODO
TODO
TODO
TODO
TODO
TODO

You will need to open two or many terminal.


## [Comparing changes](https://github.com/swsnu/os-team7/compare/base...proj4)
* The above comparing changes show 1800+ files change. (Because of adding e2fsprogs files)
* The below link show comparing changes without e2fsprogs files
## [Comparing changes (Before adding e2fsprogs)](https://github.com/swsnu/os-team7/compare/base...c6ee4f87bfc074ae929cd2f4f22eb5d619cee2e3)
## [Comparing changes (After adding e2fsprogs)](https://github.com/swsnu/os-team7/compare/cb0c31037407569a7c035be29ecdcf3b46911e7b...proj4)

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
* [fs/ext2/inode.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/inode.c)

## Other modified files to set gps location
* [fs/attr.c](https://github.com/swsnu/os-team7/blob/proj4/fs/attr.c)
* [fs/binfmt_misc.c](https://github.com/swsnu/os-team7/blob/proj4/fs/binfmt_misc.c)
* [fs/buffer.c](https://github.com/swsnu/os-team7/blob/proj4/fs/buffer.c)
* [fs/ext2/dir.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/dir.c)
* [fs/ext2/ialloc.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/ialloc.c)
* [fs/ext2/inode.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/inode.c)
* [fs/ext2/super.c](https://github.com/swsnu/os-team7/blob/proj4/fs/ext2/super.c)
* [fs/libfs.c]((https://github.com/swsnu/os-team7/blob/proj4/fs/libfs.c)
* [fs/pipe.c](https://github.com/swsnu/os-team7/blob/proj4/fs/pipe.c)
* [fs/splice.c](https://github.com/swsnu/os-team7/blob/proj4/fs/splice.c)
* [kernel/gps.c](https://github.com/swsnu/os-team7/blob/proj4/kernel/gps.c)
* [mm/filemap.c](https://github.com/swsnu/os-team7/blob/proj4/mm/filemap.c)
* [mm/filemap_xip.c](https://github.com/swsnu/os-team7/blob/proj4/mm/filemap_xip.c)
* [mm/memory.c](https://github.com/swsnu/os-team7/blob/proj4/mm/memory.c)


## Any lessons learned
* TODO
* TODO
* TODO

