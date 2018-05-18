#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

void Factorization(int n){
    int i;
    for(i=2; i<=n; i++){
        while(n % i == 0){
            n /= i;
            printf("%d", i);
            if(n != 1) printf(" * ");
        }
		puts("");
    }
}
