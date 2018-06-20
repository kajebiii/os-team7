# Project 0

* Assigned: 2018-03-07 Wednesday 11:00:00 KST
* **Due: 2018-03-14 Wednesday 20:59:59 KST**

## Introduction

This assignment will help you setup your kernel development environment. This project includes building kernel image from source code and flashing your device. After this project, the device will work with your kernel.

This is an individual project. You have to take a screenshot of terminal when you use the `lthor` utility. Send the screenshot to the TAs via email ([os-tas@spl.snu.ac.kr](mailto:os-tas%40spl.snu.ac.kr)) before the deadline.
The screenshot should look like:

![Project0Sample](/doc/assets/Project0Sample.png)

## Your Device

The development device you will use throughout this semester is ARTIK 10. The kit you received should contain the following:

* ARTIK 10 development board
* USB-A to USB-A cable
* USB-A to Micro USB-B cable
* Power supply cable/adapter
* Wireless antennas (x4)

If something is missing or malfunctioning, contact TAs immediately via email ([os-tas@spl.snu.ac.kr](mailto:os-tas%40spl.snu.ac.kr)). You must return the kit when this course ends. So please take care when you use the device and try not to lose something.

**These items will be checked before you return them.**


## Connecting the Device

![ARTIK10](/doc/assets/ARTIK10.png)

1. Plug the power supply cable to the port('3' in the picture).
1. Connect the USB-A to USB-A cable to the ports on your PC and the board('5' in the picture).
1. Connect the USB-A to Micro USB-B cable to the ports on your PC and the board('6' in the picture).
1. Turn on the power switch('2' in the picture).

The upper push switch('1' in the picture) is the power button, and the other('4' in the picture) is the reset button.

## Development Environment

To build kernel images and flash the device on your PC, development environment must be prepared. There are number of ways to do this. Choose the most appropriate one for you.

### Using Prebuilt Packages for Ubuntu

* This is the recommended way if you own a PC that runs Ubuntu.
* You can install Ubuntu side-by-side with existing OS on your machine, or erase existing OS and install Ubuntu on it. (Learn more [#1](https://help.ubuntu.com/lts/installation-guide/amd64/index.html) [#2](https://help.ubuntu.com/community/WindowsDualBoot))

[Learn more about using prebuilt packages for Ubuntu](/doc/SetupUsingPPA.md)

### Running Docker Container on GNU/Linux

You can choose this way if you own a Linux machine.

[Learn more about running Docker container](/doc/SetupDocker.md)

### Setup on Windows PC

You can choose this way if your PC runs Windows.

[Learn more about setup procedure on Windows PC](/doc/SetupOnWindows.md)

### Setup on Mac

You can choose this way if you own a Mac.

[Learn more about setup procedure on Mac](/doc/SetupOnMac.md)

### Generic Instructions on GNU/Linux

Choose this way if you own a Linux machine and one of the following is true:

* I don't want prebuilt packages or Docker.
* I'd like to hack what's behind the packages :)

[Generic Instructions on GNU/Linux](/doc/SetupManual.md)

## Compiling the Kernel & Flashing the Device

Now it's time to actually build the kernel image and flash the device.

* If you are using Docker, don't forget to execute commands in container. (Type commands after attaching to the container)
* If you use Virtual Machine, don't forget to execute commands in VM.

### Getting Kernel Source

You clone the kernel source using `git`. Execute the following command.
```bash
git clone https://github.com/swsnu/linux-3.10-artik.git
```

Or if you've [registered your ssh key to GitHub](https://help.github.com/articles/connecting-to-github-with-ssh/), you can execute the following command.
```bash
git clone git@github.com:swsnu/linux-3.10-artik.git
```

You don't have to type in GitHub username and password if you're cloing repository over ssh.

### Compiling the Kernel

Walk into the kernel source.
```bash
cd linux-3.10-artik
```

And just type
```bash
./build
```
to build the kernel image.

### Flashing the Device

![Thordown](/doc/assets/Win09Thordown.PNG)

After the building is done,

1. Open the debug console. (Use `screen` command or PuTTY)
1. Hit reset button on the device.
1. Push and hold power button for 1 second.
1. You'll see `Hit any key to stop autoboot`. Press any key.
1. Type `thordown` and <kbd>Enter</kbd>. The device will enter thordown mode.

* If you're using docker, type <kbd>Ctrl</kbd>+<kbd>a</kbd>, <kbd>d</kbd> to return to the shell.
* If you're using virtual machine, execute the following procedure in virtual machine.

Just type

```bash
lthor image.tar
```

to flash the device using the image built. Take a screenshot of flashing process and submit to TAs.

## Playing with the Device

You can log in to your device as `root`, with password `tizen`.

After logging in, you may execute `direct_set_debug.sh --sdb-set` to enable sdb. You can now execute `sdb pull`, `sdb push`, `sdb root`, and `sdb shell` command to transfer files between the device and your PC and open another command line prompt.

## We're Here to Help You

Any troubles? Discussions on [issue board](https://github.com/swsnu/osspr2018/issues) is more than welcome. Remember, we are here to help you.

Start early, ask for help if needed, and most importantly, have fun!
