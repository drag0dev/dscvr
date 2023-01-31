#include "masking.h"
#include "ip.h"
#include <stdlib.h>

unsigned char createMask(int maskLen) {
    unsigned char mask = 0;
    for (int i = 0; i < 8; ++i) {
        mask = mask << 1;
        if (maskLen > 0) {
            ++mask;
            --maskLen;
        }
    }
    return mask;
}

IPv4* getNetworkAddress(IPv4* src, unsigned int maskLen) {
    IPv4* res = malloc(sizeof(IPv4));
    if (res == NULL) return NULL;

    unsigned char mask;
    for (int i = 0; i < 4; ++i) {
        if (maskLen > 0) {
            mask = createMask(maskLen);
            if (maskLen >= 8) maskLen -= 8;
            else maskLen = 0;
        } else mask = 0;
        res->octets[i] = src->octets[i] & mask;
    }
    return res;
}

IPv4* getBroadcastAddress(IPv4* src, unsigned int maskLen) {
    IPv4* res = malloc(sizeof(IPv4));
    if (res == NULL) return NULL;

    unsigned char mask;
    for (int i = 0; i < 4; ++i) {
        if (maskLen > 0) {
            mask = createMask(maskLen);
            if (maskLen >= 8) maskLen -= 8;
            else maskLen = 0;
        } else mask = 0;
        // by inverting the mask for network addr we get mask for broadcast addr
        mask = ~mask;
        res->octets[i] = src->octets[i] | mask;
    }
    return res;
}

int incrementIp(IPv4* src, IPv4* broadcast) {
    // increment is achived by adding 1 to least signifact octet (last)
    // and adding 0 with carry the the rest of the octets going from lowest to most significant

    // add one to the least significant octet
    __asm__( "addb $1, %0;"
             "jnz end;"
             "movb $255, %1;" // in case of overflow return 255 back
             "end:"
            : "=r" (src->octets[3]) // input
            : "r" (src->octets[3]) // output
            :
    );

    // adding 0 with carry to the rest
    __asm__( "adcb $0, %0;"
             "jnz end_one;"
             "movb $255, %0;"
             "end_one:"
            : "=r" (src->octets[2])
            : "r" (src->octets[2])
    );

    __asm__( "adcb $0, %0;"
             "jnz end_two;"
             "movb $255, %0;"
             "end_two:"
            : "=r" (src->octets[1])
            : "r" (src->octets[1])
    );

    __asm__( "adcb $0, %0;"
             "jnz end_three;"
             "movb $255, %0;"
             "end_three:"
            : "=r" (src->octets[0])
            : "r" (src->octets[0])
    );

    // check if its broadcast
    for (int i = 0; i < 4; ++i) {
        if (src->octets[i] != broadcast->octets[i]) return 1;
    }
    return -1;
}
