#include "ip.h"

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
