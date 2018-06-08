
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>

struct gps_location {
  int lat_integer;
  int lat_fractional;
  int lng_integer;
  int lng_fractional;
  int accuracy;
};

int main(int argc, char **args) {
    struct gps_location result;
	int fd;
	char* data;
    if(argc != 2) {
        printf("usage: %s <filepath>\n", args[0]);
        return 0;
    }
    

	fd = open(args[1], O_RDWR | O_NONBLOCK);
	data = mmap(NULL, 1, PROT_WRITE, MAP_SHARED, fd, 0x00000000);
	*data = 'z';
	munmap(data, 1);
	close(fd);

    return 0;
}
