#pragma once
#include<time.h>
#include<stdint.h>
#define MAX_TABLE_ENTRIES 1024
#define NOT_FOUND -1
#include "ethernet.h"

typedef struct mac_table_entry{
    uint8_t mac[6];
    uint8_t port;//one switch can have max 48 physical ports so only 8 bits needed mark switch number
    time_t last_seen;
}mac_table_entry;

typedef struct forwarding_table{
    mac_table_entry table[MAX_TABLE_ENTRIES];
    uint16_t count;//16 bits required as count can be as high as 1024
}forwarding_table;

void table_init(forwarding_table *t);

//in table lookup we return the mac addr or a value saying not found throughout the pgm
//int is possible as can return -1 as well as uint8_t type of the port assctd with that mac
int table_lookup(forwarding_table * t,uint8_t mac[6]);

void table_learn(forwarding_table *t, uint8_t mac[6], uint8_t port);

void swap_expired(forwarding_table *t,int index);

void table_age(forwarding_table *t, uint32_t max_age_seconds);

void switch_process_frame(forwarding_table *t,ethernet_frame *frame,uint8_t in_port,uint8_t num_ports);