#include<stdio.h>

int main(void){
    unsigned char ch_buffer;
    while(fread(&ch_buffer,1,1,stdin)){
        printf("%c",ch_buffer);
    }
    printf("\n");
    return 0;
}