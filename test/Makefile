CC=arm-linux-gnueabi-gcc

OBJS = trial.o selector.o rotd.o
TARGET = trial selector rotd

INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all: trial selector rotd
trial: 
selector: 
rotd:

build:
	cd .. && ./build

lthor:
	cd .. && lthor image.tar

run:
	sdb root on && sdb push selector /root/selector && sdb push trial /root/trial && sdb push rotd /root/rotd

clean:
	rm -r $(TARGET)
