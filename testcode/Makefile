CC=arm-linux-gnueabi-gcc

OBJS = test.o
TARGET = test
INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all:$(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS)

build:
	cd .. && ./build

lthor:
	cd .. && lthor image.tar

run:
	sdb root on && sdb push test /root/test && sdb shell /root/test

clean:
	rm -r $(OBJS) $(TARGET)
