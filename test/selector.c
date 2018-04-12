#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
void delay(clock_t n){
    clock_t start = clock();
    while(clock() - start < n);
}
int main(int argc, char *argv[]){
    int n = atoi(argv[1]);
    while(1){
        FILE *fp = fopen("input","w");
        printf("%d\n",n);
        fprintf(fp, "%d\n",n);
        n++;
        delay(1000);
    }
}
