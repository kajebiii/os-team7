CC=arm-linux-gnueabi-gcc

OBJS=test.o
TARGET=test
INCLUDE=~/os-team7/include

.SUFFIXES : .c .o

all:$(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -I $(INCLUDE) -o $@ $(OBJS)

clean:
	rm -r $(OBJS) $(TARGET)
