#include<stdio.h>
#include "ethernet.h"
#include<string.h>
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

void parse_frame(uint8_t *raw_bytes, size_t len, ethernet_frame *out) {
    printf("\n=== Parsing Incoming Ethernet Frame (%zu bytes) ===\n",len);

    // Minimum frame size check (6 + 6 + 2 + 46 + 4 = 64 bytes)
    if (len < 64) {
        printf("[ERROR] Frame too short to be a valid Ethernet frame (%zu bytes).\n", len);
        return;
    }
    if (len > sizeof(ethernet_frame)) {
        printf("[ERROR] Frame length exceeds maximum structural capacity.\n");
        return;
    }

    // Cast raw bytes into the struct layout,possible as bytes are tightly packed in oorder
    ethernet_frame *incoming = (ethernet_frame *)raw_bytes;
    //COOL THING TO NOTE and rememmber is that this correctly maps evrythng like macs and etherype
    //but crc and payload will only be mapped correctly if payload is exactly 1500 bytes or else
    //the crc being final 4 bytes in incoming data will be consiered as payload
    //and garbage values from offset 1514,1515,1516,1517 will be taken as crc
    //below we will fix them


    //(total length minus 14 bytes of headers(6+6+2) and 4 bytes of CRC)
    size_t actual_payload_len = len - 14 - 4;

    // the CRC is at raw_bytes[len - 4]
    uint32_t wire_crc;
    memcpy(&wire_crc, &raw_bytes[len - 4], 4);

    // 4. Calculate expected CRC over headers + payload only
    size_t bytes_to_checksum = len - 4;
    uint32_t expected_crc = compute_crc32(raw_bytes, bytes_to_checksum);

    //filling output structure for upper layers,out adress we we are storing this incoming raw data
    //stored in  out ,same as using return remember that rigth?
    memcpy(out->dest_mac, incoming->dest_mac, 6);
    memcpy(out->src_mac, incoming->src_mac, 6);
    out->ethertype = incoming->ethertype; // Keep in network byte order internally...not printing yet so in network format
    memcpy(out->payload, incoming->payload, actual_payload_len);
    out->crc = wire_crc;

    // Validation Alert
    if (wire_crc != expected_crc) {
        printf("[WARNING] CRC Checksum Mismatch!\n");
        printf("  -> Wire CRC:     0x%08x\n", wire_crc);
        printf("  -> Expected CRC: 0x%08x (DATA CORRUPTED)\n", expected_crc);
    } else {
        printf("[SUCCESS] CRC Verified! Frame integrity intact (0x%08x).\n", wire_crc);
        print_frame(out);
    }
}
//this is to make the data send more better suited for all sizes as this makes crc right after 
//payload
size_t serialize_frame(ethernet_frame *frame, uint8_t *out, uint16_t wire_payload_len) {
    size_t offset = 0;

    // Copy Destination MAC (6 bytes)
    memcpy(out + offset, frame->dest_mac, 6);
    offset += 6;

    // Copy Source MAC (6 bytes)
    memcpy(out + offset, frame->src_mac, 6);
    offset += 6;

    // Copy EtherType (2 bytes)
    memcpy(out + offset, &frame->ethertype, 2);
    offset += 2;

    // Copy the payload straight out of the struct. 
    // No padding logic needed here; build_frame already guaranteed it is at least 46 bytes.
    memcpy(out + offset, frame->payload, wire_payload_len);
    offset += wire_payload_len;

    // Append CRC-32 directly after the wire payload
    memcpy(out + offset, &frame->crc, 4);
    offset += 4;

    return offset; // Total wire length (e.g., 64 bytes)
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
//in our parse frame we faces trouble as the struct always forces the crc to be from offset 1514 onwards
//so when we parsed a smaller string it couldnt find the actual crc 
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
