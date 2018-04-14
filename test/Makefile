CC=arm-linux-gnueabi-gcc

OBJS = trial.o selector.o
TARGET = trial selector
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
	sdb root on && sdb push selector /root/selector && sdb push trial /root/trial"

clean:
	rm -r $(OBJS) $(TARGET)
