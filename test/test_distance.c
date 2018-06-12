
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

struct gps_location {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};
char *filename;
struct gps_location current_location, file, temp, file_result;
int ITER;


int file_loc() {
    struct gps_location result;
    
    int err = syscall(381, filename, &result);

	if(err < 0) {
        printf("File_Loc fail!\n");
		printf("Error %d, %s\n", errno, strerror(errno));
		return 0;
	}else{
		file_result = result;
    	return 1;
	}

}

int gps_update_with_loc(struct gps_location data) {
    int err = syscall(380, &data);

    if(err < 0){
        printf("GpsUpdate fail!\n");
		printf("Error %d, %s\n", errno, strerror(errno));
		return 0;
    }
    else {
        //printf("Update success!\n");
		return 1;
    }

}
int gps_update() {
    struct gps_location data;
    
    printf("Enter latitude integer!\n");
    scanf("%d", &data.lat_integer);
	if(data.lat_integer == -5) return -1;

    printf("Enter latitude fractional!\n");
    scanf("%d", &data.lat_fractional);

    printf("Enter longitude integer!\n");
    scanf("%d", &data.lng_integer);

    printf("Enter longitude fractional!\n");
    scanf("%d", &data.lng_fractional);

    printf("Enter accuracy!\n");
    scanf("%d", &data.accuracy);
    
    int err = syscall(380, &data);

    if(err < 0){
        printf("GpsUpdate fail!\n");
		printf("Error %d, %s\n", errno, strerror(errno));
		return 0;
    }
    else {
        printf("Update success!\n");
		return 1;
    }

}



double getDis(struct gps_location ga, struct gps_location gb){
    int gax_int = ga.lat_integer;
    int gax_frac = ga.lat_fractional;
    int gay_int = ga.lng_integer;
    int gay_frac = ga.lng_fractional;

    int gbx_int = gb.lat_integer;
    int gbx_frac = gb.lat_fractional;
    int gby_int = gb.lng_integer;
    int gby_frac = gb.lng_fractional;

    int M = 1000000;
	double PI = atan2(1, 1) * 4;
    long long R = 6400000;
    long long L = 20000000;
    double xx1 = (gax_int + (1.*gax_frac / M)) * PI / (180) + (PI/2);
    double yy1 = (gay_int + (1.*gay_frac / M)) * PI / (180);
    double xx2 = (gbx_int + (1.*gbx_frac / M)) * PI / (180) + (PI/2);
    double yy2 = (gby_int + (1.*gby_frac / M)) * PI / (180);

	double gax = sin(xx1) * cos(yy1);
	double gay = sin(xx1) * sin(yy1);
	double gaz = cos(xx1);
	double gbx = sin(xx2) * cos(yy2);
	double gby = sin(xx2) * sin(yy2);
	double gbz = cos(xx2);

	double ip = gax * gbx + gay * gby + gaz * gbz; // = cos theta
	double theta = acos(ip);
	return theta * R;
}

void getInput(struct gps_location *loc, int isRand) {
	if(isRand == 0) {
		scanf("%d", &(loc->lat_integer));
		scanf("%d", &(loc->lat_fractional));
		scanf("%d", &(loc->lng_integer));
		scanf("%d", &(loc->lng_fractional));
		scanf("%d", &(loc->accuracy));
	}else if(isRand == 1) { // -45 ~ 45
		int a = rand() % (90 * 1000000 + 1);
		loc->lat_integer = a / 1000000 - 45;
		loc->lat_fractional = a % 1000000;
		int b = rand() % (360 * 1000000 + 1);
		loc->lng_integer = b / 1000000 - 180;
		loc->lng_fractional = b % 1000000;
		loc->accuracy = rand() % (10000);
	}else if(isRand == 2) { // all
		int a = rand() % (180 * 1000000 + 1);
		loc->lat_integer = a / 1000000 - 90;
		loc->lat_fractional = a % 1000000;
		int b = rand() % (360 * 1000000 + 1);
		loc->lng_integer = b / 1000000 - 180;
		loc->lng_fractional = b % 1000000;
		loc->accuracy = rand() % (10000);
	}else if(isRand == 3) {
		int a = rand() % (180 * 1000000 + 1);
		loc->lat_integer = a / 1000000 - 90;
		loc->lat_fractional = 0;//a % 1000000;
		int b = rand() % (360 * 1000000 + 1);
		loc->lng_integer = b / 1000000 - 180;
		loc->lng_fractional = 0;//b % 1000000;
		loc->accuracy = rand() % (10000);
	}
}
void getInput_n(struct gps_location *loc, int isRand, int x, int y, int d) {
	if(isRand == 3) { // near x, y
		x += 90; y += 180;
		int a, b;
		do{a = x + rand() % (2*d+1) - d; }while(0 <= a && a <= 180 * 1000000);
		loc->lat_integer = a / 1000000 - 90;
		loc->lat_fractional = a % 1000000;
		do{b = y + rand() % (2*d+1) - d; }while(0 <= b && b <= 360 * 1000000);
		loc->lng_integer = b / 1000000 - 180;
		loc->lng_fractional = b % 1000000;
		loc->accuracy = rand() % (10000);
	}
}

void printLoc(struct gps_location *loc_p) {
	struct gps_location loc = *loc_p;
	printf("%d\n", loc.lat_integer);
	printf("%d\n", loc.lat_fractional);
	printf("%d\n", loc.lng_integer);
	printf("%d\n", loc.lng_fractional);
	printf("%d\n", loc.accuracy);
}
void setting() {
	int fd;
	if(file_loc() == 0) exit(1);
	temp = file_result;
	temp.accuracy = 200000000;
	gps_update_with_loc(temp);
	fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC);
	if(fd == -1) exit(-1);
	close(fd);
	temp = file;
	gps_update_with_loc(temp);
	fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC);
	if(fd == -1) exit(-1);
	close(fd);
	gps_update_with_loc(current_location);
}
void test() {
	setting();
	int fd = (open(filename, O_CREAT|O_WRONLY|O_TRUNC));
	int kernel = (fd == -1 ? 0 : 1);
	close(fd);
	//geo_permission(file); TODOTODOTODOTODOTODO
	double dis = getDis(current_location, file);
	long long acc = current_location.accuracy + file.accuracy;
	int answer = (dis <= acc);
	if(kernel != answer) {
		printf("kernel %d | answer %d\n", kernel, answer);
		printf("acc %lld | dis %f\n", acc, dis);
		printLoc(&current_location); puts("");
		printLoc(&file);
		exit(0);
	}
}
void my_test() {
	int tc, x, y;
	int DITER = ITER / 10;
	if(DITER == 0) DITER = 1;
	//180 * 360;
	srand(clock());
	for(tc=0; tc<ITER; tc++) {
		getInput(&current_location, 1);
		getInput(&file, 1);
		test();
		getInput(&current_location, 2);
		getInput(&file, 2);
		test();
		getInput(&current_location, 3);
		getInput(&file, 3);
		test();
		if(tc % DITER == 0) 
			printf("random %d finish\n", tc);
	}
	puts("random ac");
	for(tc=0; tc<ITER; tc++) {
		getInput(&current_location, 2);
		getInput(&file, 2);
		long long acc;
		int kernel, answer;
		double dis = getDis(current_location, file);

		int less = dis * 0.5;
		int big = dis * 1.5;
		file.accuracy = 0;
		current_location.accuracy = less;
		test();

		file.accuracy = 0;
		current_location.accuracy = big;
		test();

		if(tc % DITER == 0) 
			printf("less big %d finish\n", tc);
	}
	puts("less big ac");
	for(x=0; x<=180; x++) {
		for(y=0; y<=360; y++) {
			for(int tc=0; tc<ITER / 180 / 360; tc++) {
				getInput_n(&current_location, 3, x, y, 1000);
				getInput_n(&file, 3, x, y, 1000);
				test();
			}
		}
		if(x%18 == 0) 
		printf("x %d finish\n", x);
	}
	puts("near point ac");
}

int main(int argc, char **args) {
	struct gps_location result;
	int fd, k;
	if(argc != 3) {
		printf("usage: %s <filepath> <iter>\n", args[0]);
		return 0;
	}
	filename = args[1];
	ITER = atoi(args[2]);
	my_test();
	/*
	while(1) {
		if(file_loc(args[1]) == 0) break;
		fd = open(args[1], O_CREAT|O_WRONLY|O_TRUNC);
		printf("%d\n", fd);
		close(fd);
		while(1) {
			k = gps_update();
			if(k != 0) break;
		}
		if(k == -1) break;
	}
	*/
	/*
	int err = syscall(381, args[1], &result);

	if(err < 0) {
		printf("File_Loc fail!\n");
		printf("Error %d, %s\n", errno, strerror(errno));
		return 0;
	}else{
		printf("latitude:\t%d.%06d\n", result.lat_integer, result.lat_fractional);
		printf("longitude:\t%d.%06d\n", result.lng_integer, result.lng_fractional);
		printf("accuracy:\t%d(m)\n\n", result.accuracy);

		printf("Google Map Link\n");
		printf("https://www.google.com/maps/@%d.%06d,%d.%06d,15z\n", result.lat_integer, result.lat_fractional, result.lng_integer, result.lng_fractional);
	}
	*/

	return 0;
}
