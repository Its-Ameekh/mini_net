#include<stdio.h>
#include<stdint.h>

typedef struct{
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    uint8_t payload[1500];
    uint32_t crc;
}__attribute__((packed)) ethernet_frame;