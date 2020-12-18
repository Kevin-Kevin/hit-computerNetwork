#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int main(){
    char str[1024*8];
    str[0] = '0';
    str[1] = '\0';
    struct tm *local;
    time_t t;
    t = time(NULL);
    local = localtime(&t);

    printf("time = %s\n",ctime(&t));



    return 0;
}