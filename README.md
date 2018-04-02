# OS Team 7 Project 2 README

## How to build kernel
The way to build kernel doesn't differ from original kernel. Type `./build` in the root folder, and kernel will be builded. Or you can type `make build` in the "test" folder.

## Flashing the Device
The way to flash the device doesn't differ from original.

After the building is done.

1. Open the debug console. (Use `screen` command or PuTTY)
1. Hit reset button on the device.
1. Push and hold power button for 1 second.
1. You'll see `Hit any key to stop autoboot`. Press any key.
1. Type `thordown` and <kbd>Enter</kbd>. The device will enter thordown mode.

Just type `lthor image.tar` in the root folder, or `make lthor` in the "test" folder.

## How to execute test code
After logging in, you have to execute `direct_set_debug.sh --sdb-set` to enable sdb.

In the "test" folder, type `make clean` and `make`.

Then type `make run`


## [Comparing changes](https://github.com/swsnu/os-team7/compare/base...proj2)

## Preprocess
* [arch/arm/include/asm/unistd.h]
* [arch/arm/include/uapi/asm/unistd.h]
* [arch/arm/kernel/calls.S]
* [kernel/Makefile]

## Something
* Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
* Sed ut perspiciatis unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam, eaque ipsa quae ab illo inventore veritatis et quasi architecto beatae vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur magni dolores eos qui ratione voluptatem sequi nesciunt. Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit, sed quia non numquam eius modi tempora incidunt ut labore et dolore magnam aliquam quaerat voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi consequatur? Quis autem vel eum iure reprehenderit qui in ea voluptate velit esse quam nihil molestiae consequatur, vel illum qui dolorem eum fugiat quo voluptas nulla pariatur?a
* But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was born and I will give you a complete account of the system, and expound the actual teachings of the great explorer of the truth, the master-builder of human happiness. No one rejects, dislikes, or avoids pleasure itself, because it is pleasure, but because those who do not know how to pursue pleasure rationally encounter consequences that are extremely painful. Nor again is there anyone who loves or pursues or desires to obtain pain of itself, because it is pain, but because occasionally circumstances occur in which toil and pain can procure him some great pleasure. To take a trivial example, which of us ever undertakes laborious physical exercise, except to obtain some advantage from it? But who has any right to find fault with a man who chooses to enjoy a pleasure that has no annoying consequences, or one who avoids a pain that produces no resultant pleasure?

## Any lessons learned
* To do
* To do
* To do
