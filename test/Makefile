CC=arm-linux-gnueabi-gcc

OBJS_1 = gpsupdate.o
TARGET_1 = gpsupdate
OBJS_2 = file_loc.o
TARGET_2 = file_loc
OBJS_3 = mmap.o
TARGET_3 = mmap
INCLUDE = -I../include
CFLAGS = $(INCLUDE)
.SUFFIXES : .c .o

all: $(TARGET_1) $(TARGET_2) $(TARGET_3)

$(TARGET_1) : $(OBJS_1)
	$(CC) -o $@ $(OBJS_1)

$(TARGET_2) : $(OBJS_2)
	$(CC) -o $@ $(OBJS_2)

$(TARGET_3) : $(OBJS_3)
	$(CC) -o $@ $(OBJS_3)

clean:
	rm -r $(OBJS_1) $(TARGET_1) $(OBJS_2) $(TARGET_2) $(OBJS_3) $(TARGET_3)

build:
	cd .. && ./build
lthor:
	cd .. && lthor image.tar
run:
	sdb root on && sdb push $(TARGET_1) /root/$(TARGET_1) && sdb push $(TARGET_2) /root/$(TARGET_2) && sdb push $(TARGET_3) /root/$(TARGET_3) && sdb push ../proj4.fs /root/proj4.fs
