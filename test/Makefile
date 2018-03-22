CC=arm-linux-gnueabi-gcc

OBJS = test_ptree.o
TARGET = test_ptree
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
	sdb root on && sdb push test_ptree /root/test_ptree && sdb shell "/root/test_ptree && pstree 0"

clean:
	rm -r $(OBJS) $(TARGET)
