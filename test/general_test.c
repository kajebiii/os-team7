
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>

struct gps_location {
  int lat_integer;
  int lat_fractional;
  int lng_integer;
  int lng_fractional;
  int accuracy;
};

int checkerror(int re) {
    if(re == -1) {
		printf("Error %d, %s, return value : %d\n", errno, strerror(errno), re);
    }
    return re;
}

void get_gps(const char *file, struct gps_location *loc) {
    int err = syscall(381, file, loc);
}

void set_gps(struct gps_location *loc) {
    syscall(380, loc);
}

void make_gps(struct gps_location *loc) {
    loc->lat_integer = rand()%180 - 90;
    loc->lng_integer = rand()%360 - 180;
    loc->lat_fractional = rand()%1000000;
    loc->lng_fractional = rand()%1000000;
    loc->accuracy = 100000000; // prevent permission deny
}

int comp_gps(struct gps_location *loc1, struct gps_location *loc2) {
    int *a=(int*)loc1, *b=(int*)loc2;
    for(int i=0; i<5; i++) if(a[i] != b[i]) return 0;
    return 1;
}

int comp(struct gps_location *loc, const char* filename) {
    struct gps_location a;
    int re;
    get_gps(filename, &a);
    re = comp_gps(&a, loc);
    if(re) printf("Same!!!!!\n");
    else printf("Wrong!!!!!\n");
    return re;
}

void make_file(const char *file) {
    int fd = checkerror(open(file, O_RDWR | O_CREAT, 0660));
    close(fd);
}

void write_write(const char *file) {
    int fd = checkerror(open(file, O_RDWR));
    write(fd, "x", 1);
    close(fd);
}

char read_read(const char *file) {
    int fd = checkerror(open(file, O_RDWR));
    char buf[10];
    read(fd, buf, 1);
    close(fd);
    return buf[0];
}

void write_mmap(const char *file) {
	int fd = checkerror(open(file, O_RDWR));
	char *data = (char*)mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x00000000), d;
    if(data == (char*)-1) {
        return checkerror(-1);
    }
	*data = 'z';
	munmap(data, 1);
	close(fd);
}

char read_mmap(const char *file) {
	int fd = checkerror(open(file, O_RDWR));
	char *data = (char*)mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x00000000), d;
    if(data == (char*)-1) {
        return checkerror(-1);
    }
    d = *data;
	munmap(data, 1);
	close(fd);
    return d;
}

int main(int argc, char **args) {
    struct gps_location g[10], f;
    
    for(int i=0; i<10; i++) make_gps(g+i);

    set_gps(g);

    make_file("a");
    comp(g, "a");
    make_file("b");
    comp(g, "b");
    

    set_gps(g+1);

    read_read("b");
    comp(g, "b");
    write_write("b");
    comp(g+1, "b");

    read_mmap("a");
    comp(g, "a");
    write_mmap("a");
    comp(g+1, "a");

    return 0;
}
