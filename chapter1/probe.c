#include <stdio.h>
#include "ethernet.h"

int main(void) {
    printf("sizeof(ethernet_frame) = %zu\n", sizeof(ethernet_frame));
    printf("sizeof without packed  = ?\n");
    return 0;
}