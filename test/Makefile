CC=arm-linux-gnueabi-gcc

OBJS = trial.o selector.o rotd.o unlockFirst.o random.o
TARGET = trial selector rotd unlockFirst random

INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all: trial selector rotd unlockFirst random
trial: 
selector: 
rotd:

build:
	cd .. && ./build

lthor:
	cd .. && lthor image.tar

run:
	sdb root on && sdb push selector /root/selector && sdb push trial /root/trial && sdb push rotd /root/rotd && sdb push unlockFirst /root/unlockFirst && sdb push random /root/random

clean:
	rm -r $(TARGET)
