#include "ip.h"
#include <stdlib.h>

void iptoo(IPv4* ip, char* ipString, int len) {
    int i = 0;
    int j = 0;
    while (i < 4 && j < len) {
        ip->octets[i] = 0;
        while (*ipString != '.' && j < len) {
            ip->octets[i] *= 10;
            ip->octets[i] += (*ipString) - 48;
            ipString++;
            j++;
        }
        ipString++;
        j++;
        i++;
    }
}

char* otoip(IPv4* ip) {
    char* res = malloc(sizeof(char)*16);
    if (res == NULL) return NULL;

    unsigned char temp;
    int j = 0;
    for(int i = 0; i < 4; ++i) {
        temp = ip->octets[i];
        char octetBuffer[] = {'0', '0', '0'};
        int k = 0;
        do {
            octetBuffer[k] = temp % 10 + 48;
            temp /= 10;
            k++;
        } while(temp != 0);
        for(k = 2; k >= 0; --k, ++j) {
            res[j] = octetBuffer[k];
        }
        res[j] = '.';
        j++;
    }
    res[j-1] = 0;
    return res;
}
