#ifndef ARP_H_
#define ARP_H_

#include "get.h"
#include "ip.h"
#include <netpacket/packet.h>
#include <netinet/if_ether.h>

#define MAC_LENGTH 6
#define IPV4_LENGTH 4

/* arp response */
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

/* getting fd for a specific interface */
int bind_arp(int* fd, int ifidx);

/* function for sending arp request */
int send_arp(int fd, char* targetIp, interfaceInfo* ifInfo, struct sockaddr_ll* addr, struct ether_arp* req);

/* function that tries reading from fd */
int read_arp(int fd);

/* function for sending arp request for every ip in the given subnet */
void* sender(void** args);

/* function that tries to read arp response indefinitely */
void* receiver(int* fd);

#endif // ARP_H_
