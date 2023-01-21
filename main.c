#include "get.h"
#include <stdint.h>
#include <string.h>

typedef struct ipv4{
    uint8_t octets[4];
} IPv4;

// TODO: cant know which interface we want, list all ips and interfacesa and ask user which one to use
// TODO: based on the interface can get default mask, and then confirm with user, if not user can enter

/* ip to octets */
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

int main () {
    char* ipString = getIP();
    unsigned int mask;
    getMask(&mask);
    IPv4 ip;
    iptoo(&ip, ipString, strlen(ipString));
    return 0;
}
