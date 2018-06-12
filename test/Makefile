CC=arm-linux-gnueabi-gcc

OBJS_1 = gpsupdate.o
TARGET_1 = gpsupdate
OBJS_2 = file_loc.o
TARGET_2 = file_loc
OBJS_3 = mmap.o
TARGET_3 = mmap
OBJS_4 = test_distance.o
TARGET_4 = test_distance
OBJS_5 = general_test.o
TARGET_5 = general_test
INCLUDE = #-I ../include
CFLAGS = $(INCLUDE) -lm
.SUFFIXES : .c .o

all: $(TARGET_1) $(TARGET_2) $(TARGET_3) $(TARGET_4) $(TARGET_5)

$(TARGET_1) : $(OBJS_1)
	$(CC) $(CFLAGS) -o $@ $(OBJS_1)

$(TARGET_2) : $(OBJS_2)
	$(CC) $(CFLAGS) -o $@ $(OBJS_2)

$(TARGET_3) : $(OBJS_3)
	$(CC) $(CFLAGS) -o $@ $(OBJS_3)

$(TARGET_4) : $(OBJS_4)
	$(CC) $(CFLAGS) -o $@ $(OBJS_4) -lm

$(TARGET_5) : $(OBJS_5)
	$(CC) -o $@ $(OBJS_5)

clean:
	rm -r $(OBJS_1) $(TARGET_1) $(OBJS_2) $(TARGET_2) $(OBJS_3) $(TARGET_3) $(OBJS_4) $(TARGET_4) $(OBJS_5) $(TARGET_5)

build:
	cd .. && ./build
lthor:
	cd .. && lthor image.tar
run:
	sdb root on
	sdb push $(TARGET_1) /root/$(TARGET_1) && sdb push $(TARGET_2) /root/$(TARGET_2) && sdb push $(TARGET_3) /root/$(TARGET_3) 
	sdb push $(TARGET_4) /root/$(TARGET_4) 
	sdb push $(TARGET_5) /root/$(TARGET_5) 
	sdb push ../proj4.fs /root/proj4.fs
