#include<stdio.h>
#include "ethernet.h"
#include<arpa/inet.h>
// Precomputes the 256-entry lookup table using the CRC-32S VAlue(did not understand comeback later)
void build_crc32_table(uint32_t table[256]) {
    for (int i = 0; i < 256; i++) {
        uint32_t n = i;
        for (int j = 0; j < 8; j++) {
            if (n & 1) {
                n = (n >> 1) ^ 0xEDB88320;
            } else {
                n = n >> 1;
            }
        }
        table[i] = n;
    }
}

// Executes table-driven standard CRC-32 division across a raw byte buffer
//this is standard algo
uint32_t compute_crc32(uint8_t *data, size_t length) {
    uint32_t table[256];
    build_crc32_table(table); // Generate the local lookup tablespace
    
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}


void build_frame(
    ethernet_frame *frame,
    uint8_t dest[6],
    uint8_t src[6],
    uint16_t ethertype,
    uint8_t *data,
    uint16_t data_len
){
    for(int i=0;i<6;i++){
        frame->dest_mac[i]=dest[i];
    }
    for(int i=0;i<6;i++){
        frame->src_mac[i]=src[i];
    }
    frame->ethertype=htons(ethertype);

    if(data_len>1500)data_len=1500;
    //now we need to copy data into the frame
    for(int i=0;i<data_len;i++){
        frame->payload[i]=data[i];
    }
    if(data_len<46){
    for(int i=data_len;i<46;i++){
        frame->payload[i]=0;
    }

    data_len=46;

    }
    // Calculate CRC over headers + active payload (offsets 0 up to the CRC boundary)
    size_t bytes_to_checksum = 6 + 6 + 2 + data_len;
    frame->crc = compute_crc32((uint8_t *)frame, bytes_to_checksum);

}

void print_mac(uint8_t mac[6]){
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}
// prints:  aa:bb:cc:dd:ee:ff
//x is the specifier for hex

void print_frame(ethernet_frame *frame){
    printf("Frame:\n");
    printf("Destination MAC:");
    print_mac(frame->dest_mac);
    printf("Source MAC:");
    print_mac(frame->src_mac);
    printf("Ethertype:0x%04x\n",ntohs(frame->ethertype));//0x initially only cosmetic as outside specifier
    
    //for testing and debug puposes we will print only the first 16 bytes of 
    //the payload as dont need 1500 bytes on terminalevrytime and we will print as hex not %c
    printf("  Payload:   ");
    for(int i = 0; i < 16; i++) {
        printf("%02x ", frame->payload[i]);
    }
    printf("...\n");
    //we have unconditonally dumped first 16 bytes as we know its atlest 46 bytes and max 1500 so no need of checking
    printf("CRC: 0x%08x\n",frame->crc);
}
