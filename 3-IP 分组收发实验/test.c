#include<stdio.h>
#include<string.h>
#include<memory.h>
#include<stdlib.h>
int main() {
    long int longInt = 0x12345678;
   
    unsigned char* p = (unsigned char*)&longInt;
   
    printf("\n longInt(%%d) = %ld \n longInt(%%x) = %lx \n print from address = ",longInt,longInt);
    for (int i = 0;i < 8;i++) {
        printf("%x ", *(p + i));
    }
    printf("\n p = %x \n", *p);
    int array[10];
    printf("\n array0 address = %u \n array1 address = %u \n", array, array + 1);

    unsigned int ip = 0x48484848;
    
    char* str = (char*)malloc(5 * sizeof(char));

    memcpy(str, &ip, 4);
    str[4]='\0';
    char c = '0';
    printf(" str = %s \n %d",str,c);
    return 0;
}