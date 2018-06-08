
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

struct gps_location {
  int lat_integer;
  int lat_fractional;
  int lng_integer;
  int lng_fractional;
  int accuracy;
};

int main(int argc, char **args) {
    struct gps_location result;
    if(argc != 2) {
        printf("usage: %s <filepath>\n", args[0]);
        return 0;
    }
    
    int err = syscall(381, args[1], &result);

	if(err < 0) {
		printf("Error %d\n", errno);
		return 0;
	}else{
		printf("latitude:\t%d.%06d\n", result.lat_integer, result.lat_fractional);
		printf("longitude:\t%d.%06d\n", result.lng_integer, result.lng_fractional);
		printf("accuracy:\t%d(m)\n\n", result.accuracy);

		printf("Google Map Link\n");
		printf("https://www.google.com/maps/@%d.%06d,%d.%06d,15z\n", result.lat_integer, result.lat_fractional, result.lng_integer, result.lng_fractional);
	}

    return 0;
}
