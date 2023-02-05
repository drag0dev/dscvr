#include "arp.h"
#include "get.h"
#include "ip.h"
#include "masking.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <sys/socket.h>

int bind_arp(int* fd, int ifidx) {
    // get fd
    *fd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ARP));
    if (*fd < 0) {
        perror("socket");
        return -1;
    }

    // interface addr
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifidx;

    if (bind(*fd, (struct sockaddr*) &sll, sizeof(struct sockaddr_ll)) < 0){
        perror("bind");
        return -1;
    }
    return 0;
}

int send_arp(int fd, char* targetIp, interfaceInfo* ifInfo) {
    struct sockaddr_ll addr;
    memset(&addr, 0, sizeof(struct sockaddr_ll));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifInfo->ifidex;
    addr.sll_halen = ETHER_ADDR_LEN;
    addr.sll_protocol = htons(ETH_P_ARP);

    // broadcast address
    const unsigned char etherBroadcastAddr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    memcpy(addr.sll_addr, etherBroadcastAddr, ETHER_ADDR_LEN);

    struct ether_arp req;
    req.arp_hrd = htons(ARPHRD_ETHER); // format of the hw address
    req.arp_pro = htons(ETH_P_IP); // format of the protocol address
    req.arp_hln = ETHER_ADDR_LEN; // hw address len
    req.arp_pln = sizeof(in_addr_t); // protocol address len
    req.arp_op = htons(ARPOP_REQUEST); // arp opcode
    memset(&req.arp_tha, 0, sizeof(req.arp_tha)); // mac address 0 - broadcast

    struct in_addr targetIPAddr;
    memset(&targetIPAddr, 0, sizeof(struct in_addr));
    inet_aton(targetIp, &targetIPAddr); // it never fails since passed ips are generated internally
    memcpy(&req.arp_tpa, &targetIPAddr.s_addr, sizeof(req.arp_tpa));

    // source ip
    memcpy(req.arp_spa, &ifInfo->ip->s_addr, sizeof(ifInfo->ip->s_addr));

    // source mac
    memcpy(&req.arp_sha, ifInfo->mac, sizeof(req.arp_sha));

    if (sendto(fd, &req, sizeof(req), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("Error sending ARP requst\n");
        return -1;
    }

    return 0;
}

int read_arp(int fd) {
    // NOTE: reply is 46 bytes long, response with any other length is assumed to be wrong
    // TODO: timeout when we cant find a device/poll?
    unsigned char buffer[1024];
    ssize_t length = recvfrom(fd, buffer, 1024, 0, NULL, NULL);

    if (length == -1) {
        perror("recvfrom()");
        return -1;
    }

    // reply is 46 bytes long, response with any other length is assumed to be something else - failed to find
    if (length != 46) return -1;

    struct arp_header *arp_resp = (struct arp_header *) buffer;

    printf("IP: %d.%d.%d.%d  ",
          arp_resp->sender_ip[0],
          arp_resp->sender_ip[1],
          arp_resp->sender_ip[2],
          arp_resp->sender_ip[3]);
    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
          arp_resp->sender_mac[0],
          arp_resp->sender_mac[1],
          arp_resp->sender_mac[2],
          arp_resp->sender_mac[3],
          arp_resp->sender_mac[4],
          arp_resp->sender_mac[5]);

    return 0;
}

/* function for sending all arp requests */
void* sender(void** args) {
    IPv4* ip = args[0];
    IPv4* broadcast = args[1];
    interfaceInfo* ifInfo = args[2];
    int* fd = args[3];
    char* ipStr;
    while (incrementIp(ip, broadcast) == 1) {
        ipStr = otoip(ip);
        send_arp(*fd, ipStr, ifInfo);
        free(ipStr);
    }
    return NULL;
}

// TODO: timeout/user input for stopping?
void* receiver(int* fd) {
    while (1) {
        read_arp(*fd);
    }
    return NULL;
}
