
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

struct gps_location {
  int lat_integer;
  int lat_fractional;
  int lng_integer;
  int lng_fractional;
  int accuracy;
};

int main(void) {
    struct gps_location data;
    
    printf("Enter latitude integer!\n");
    scanf("%d", &data.lat_integer);

    printf("Enter latitude fractional!\n");
    scanf("%d", &data.lat_fractional);

    printf("Enter longitude integer!\n");
    scanf("%d", &data.lng_integer);

    printf("Enter longitude fractional!\n");
    scanf("%d", &data.lng_fractional);
    
    int err = syscall(380, &data);

    if(err < 0){
        printf("Update fail!\n");
        printf("%s\n", strerror(errno));
    }
    else {
        printf("Update success!\n");
    }

    return 0;
}
