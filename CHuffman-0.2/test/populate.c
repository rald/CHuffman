#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double drand() {
    return rand()/(RAND_MAX+1.0);
}

int rnd(int x) {
    return (int)(drand()*x);
}

void repeat(int n,int ch) {
    int i;
    for(i=0;i<n;i++) putchar(ch);
}

int main(void) {
    int i,n1;
    srand(time(NULL));
    n1=rnd(256);
    for(i=0;i<n1;i++) {
        repeat(rnd(1024),rnd(26)+65);
    }
    return 0;
}
