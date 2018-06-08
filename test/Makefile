CC=arm-linux-gnueabi-gcc

OBJS_1 = gpsupdate.o
TARGET_1 = gpsupdate
OBJS_2 = file_loc.o
TARGET_2 = file_loc
INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1) : $(OBJS_1)
	$(CC) -o $@ $(OBJS_1)

$(TARGET_2) : $(OBJS_2)
	$(CC) -o $@ $(OBJS_2)

clean:
	rm -r $(OBJS_1) $(TARGET_1) $(OBJS_2) $(TARGET_2)
