#ifndef ARP_H_
#define ARP_H_

#include "get.h"
#include "ip.h"
#include <stdint.h>

#define MAC_LENGTH 6
#define IPV4_LENGTH 4

struct arp_header {
    unsigned short hardware_type;
    unsigned short protocol_type;
    unsigned char hardware_len;
    unsigned char protocol_len;
    unsigned short opcode;
    unsigned char sender_mac[MAC_LENGTH];
    unsigned char sender_ip[IPV4_LENGTH];
    unsigned char target_mac[MAC_LENGTH];
    unsigned char target_ip[IPV4_LENGTH];
};

int bind_arp(int* fd, int ifidx);
int send_arp(int fd, char* targetIp, interfaceInfo* ifInfo);
int read_arp(int fd);
void sender(IPv4* ip, IPv4* broadcast, interfaceInfo* ifInfo, int* fd);
void receiver(int* fd);

#endif // ARP_H_
