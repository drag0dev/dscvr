#include "arp.h"
#include "masking.h"
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
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
        printf("Error creating a new socket: ");
        perror("socket");
        return -1;
    }

    // interface addr
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(struct sockaddr_ll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifidx;

    if (bind(*fd, (struct sockaddr*) &sll, sizeof(struct sockaddr_ll)) < 0){
        printf("Error binding: ");
        perror("bind");
        return -1;
    }
    return 0;
}

int send_arp(int fd, char* targetIp, struct sockaddr_ll* addr, struct ether_arp* req) {
    struct in_addr targetIPAddr;
    memset(&targetIPAddr, 0, sizeof(struct in_addr));
    inet_aton(targetIp, &targetIPAddr); // it never fails since passed ips are generated internally
    memcpy(&req->arp_tpa, &targetIPAddr.s_addr, sizeof(req->arp_tpa));

    if (sendto(fd, req, sizeof(*req), 0, (struct sockaddr*)addr, sizeof(*addr)) == -1) {
        printf("Error sending ARP requst\n");
        return -1;
    }
    return 0;
}

int read_arp(int fd) {
    unsigned char buffer[1024];
    ssize_t length = recvfrom(fd, buffer, 1024, 0, NULL, NULL);

    if (length == -1) {
        perror("recvfrom()");
        return -1;
    }
    // reply length can change due to packets being tagget/not tagged, no point expecting a certain length
    // the arp response it self is atleast 28 bytes, anything shorter is ignored
    if (length < 28) return -1;

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

void makeStructs(struct sockaddr_ll* addr, struct ether_arp* req, interfaceInfo* ifInfo) {
    // sockaddr_ll
    memset(addr, 0, sizeof(struct sockaddr_ll));
    addr->sll_family = AF_PACKET;
    addr->sll_ifindex = ifInfo->ifidex;
    addr->sll_halen = ETHER_ADDR_LEN;
    addr->sll_protocol = htons(ETH_P_ARP);
    const unsigned char etherBroadcastAddr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    memcpy(addr->sll_addr, etherBroadcastAddr, ETHER_ADDR_LEN);

    // ether_arp
    req->arp_hrd = htons(ARPHRD_ETHER); // format of the hw address
    req->arp_pro = htons(ETH_P_IP); // format of the protocol address
    req->arp_hln = ETHER_ADDR_LEN; // hw address len
    req->arp_pln = sizeof(in_addr_t); // protocol address len
    req->arp_op = htons(ARPOP_REQUEST); // arp opcode
    memset(&req->arp_tha, 0, sizeof(req->arp_tha)); // mac address 0 - broadcast
    memcpy(req->arp_spa, &ifInfo->ip->s_addr, sizeof(ifInfo->ip->s_addr)); // source ip
    memcpy(&req->arp_sha, ifInfo->mac, sizeof(req->arp_sha)); // source mac
}

/* function for sending all arp requests */
void* sender(void** args) {
    IPv4* ip = args[0];
    IPv4* broadcast = args[1];
    interfaceInfo* ifInfo = args[2];
    int* fd = args[3];
    char* ipStr;

    // avoid making these structures every time
    struct sockaddr_ll addr;
    struct ether_arp req;
    makeStructs(&addr, &req, ifInfo);

    while (incrementIp(ip, broadcast) == 1) {
        ipStr = otoip(ip);
        send_arp(*fd, ipStr, &addr, &req);
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
