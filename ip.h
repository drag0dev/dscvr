#ifndef IP_H_
#define IP_H_

#include <stdint.h>

typedef struct ipv4{
    uint8_t octets[4];
} IPv4;

/* ip to octals (ipv4 struct) */
void iptoo(IPv4* ip, char* ipString, int len);

#endif // IP_H_
