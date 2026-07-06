#include<stdio.h>
#include"switch.h"
#include<time.h>

int main(void){
    forwarding_table d;
    table_init(&d);//dont forget to initialise or count will have garbage values

    printf("============USING THE SWITCH NOW============\n");

    ethernet_frame frame1={
        {0Xff,0XFF,0Xff,0Xff,0Xff,0Xff},
        {0XAA,0XFF,0XDD,0XBB,0XCC,0X11},
        
        0x8000,"hello,world!",1234
    };

    switch_process_frame(&d,&frame1,1,3);

    ethernet_frame frame2={
        {0XAA,0XFF,0XDD,0XBB,0XCC,0X11},
        {0XAA,0XFF,0XDD,0XBB,0X7C,0X51},
        
        0x8000,"hello,world!",1345
    };
    switch_process_frame(&d,&frame2,1,3);

    ethernet_frame frame3={
        {0XAA,0XFF,0XDD,0XBB,0XCC,0X11},
        {0XAA,0XFF,0XDD,0XBB,0X7C,0X51},
        
        0x8000,"hello,world!",12345
    };
    switch_process_frame(&d,&frame3,3,4);


    return 0;
}