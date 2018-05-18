CC=arm-linux-gnueabi-gcc

OBJS = test.o trial.o
TARGET = test trial

INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all: test trial
test:
trial:

build:
	cd .. && ./build

lthor:
	cd .. && lthor image.tar

run:
	sdb root on && sdb push test /root/test && sdb push trial /root/trial

#sdb root on && sdb push selector /root/selector && sdb push trial /root/trial && sdb push rotd /root/rotd && sdb push unlockFirst /root/unlockFirst && sdb push random /root/random

clean:
	rm -r $(TARGET)
