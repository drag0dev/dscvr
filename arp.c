#include "arp.h"
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

int send_arp(int fd, char* ip, int ifidx) {
    struct sockaddr_ll addr;
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifidx;
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
    memset(&req.arp_tha, 0, sizeof(req.arp_tha));

    struct in_addr targetIPAddr;
    memset(&targetIPAddr, 0, sizeof(struct in_addr));
    inet_aton(ip, &targetIPAddr); // it never fails since passed ips are generated internally
    memcpy(&req.arp_tpa, &targetIPAddr.s_addr, sizeof(req.arp_tpa));

    if (sendto(fd, &req, sizeof(req), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("Error sending ARP requst\n");
        return -1;
    }

    return 0;
}
