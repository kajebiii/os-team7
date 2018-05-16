CC=arm-linux-gnueabi-gcc

OBJS = test.o 
TARGET = test

INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all: test
test:

build:
	cd .. && ./build

lthor:
	cd .. && lthor image.tar

run:
	sdb root on && sdb push test /root/test

#sdb root on && sdb push selector /root/selector && sdb push trial /root/trial && sdb push rotd /root/rotd && sdb push unlockFirst /root/unlockFirst && sdb push random /root/random

clean:
	rm -r $(TARGET)
