#include<stdio.h>
#include<time.h>

void delay(clock_t n){
    clock_t start = clock();
    while(clock() - start < n);
}

int main(int argc, char *argv[]){
    while(1){
        int n, i;
        FILE *fp = fopen("integer","r");
        fscanf(fp, "%d",&n);
        for(i=2;i<=n;i++){
            while(n%i==0){
                n/=i;
                printf("%d",i);
                if(n!=1){
                    printf(" * ");
                }
            }
        }
        delay(1000);
    }
}
