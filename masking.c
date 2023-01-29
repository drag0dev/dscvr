#include "masking.h"
#include "ip.h"
#include <stdlib.h>
#include <string.h>

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

    unsigned short mask;
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
