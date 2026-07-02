#pragma once
#include<stdint.h>
#include <stddef.h>

typedef struct{
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
    uint8_t payload[1500];
    uint32_t crc;
}__attribute__((packed)) ethernet_frame;

//for crc
void build_crc32_table(uint32_t table[256]);
uint32_t compute_crc32(uint8_t *data, size_t length);

// Interface Prototypes
void build_frame(
    ethernet_frame *frame,
    uint8_t dest[6],
    uint8_t src[6],
    uint16_t ethertype,
    uint8_t *data,
    uint16_t data_len
);



void print_mac(uint8_t mac[6]);
void print_frame(ethernet_frame *frame);