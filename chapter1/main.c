#include "ethernet.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    ethernet_frame frame;

    uint8_t dest[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    uint8_t src[6]  = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    uint8_t data[]  = "Hello, Ethernet!";

    build_frame(&frame, dest, src, 0x0800, data, sizeof(data));
    print_frame(&frame);
    
    uint16_t wire_payload_len = sizeof(data) < 46 ? 46 : sizeof(data);

    uint8_t raw[1518];
    size_t frame_len = serialize_frame(&frame, raw, wire_payload_len);

    ethernet_frame parsed;
    printf("parsed frame is under\n");
    parse_frame(raw, frame_len, &parsed);

    return 0;
}