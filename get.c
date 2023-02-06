#include "get.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>
#include <netpacket/packet.h>

void clearSTDIN(){
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void getMask(unsigned int* mask) {
    while (1) {
        // get user input
        printf("Enter mask (0-31): ");
        scanf("%u", mask);
        clearSTDIN();
        if ((*mask) >= 0 && (*mask) <= 31) break; // mask can only be [0, 31]
    }
}

interfaceInfo* getInterface() {
    struct ifaddrs *addrs, *tmp;

    if (getifaddrs(&addrs) != 0) {
        perror("getifaddrs");
        return NULL;
    }

    printf("Available interfaces:\n");
    tmp = addrs;
    int numberOfInterfaces = 0;
    while (tmp){
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
            printf("(%d): %s\n", numberOfInterfaces, tmp->ifa_name);
            numberOfInterfaces++;
        }
        tmp = tmp->ifa_next;
    }
    printf("------------------------------\n");

    int option;
    while (1) {
        printf("Choose an interface: ");
        scanf("%d", &option);
        clearSTDIN();
        if (option >= 0 && option < numberOfInterfaces) break;
    }

    interfaceInfo* res = malloc(sizeof(interfaceInfo));
    if (res == NULL) return NULL;
    res->ip = malloc(sizeof(struct in_addr));
    if (res->ip == NULL) {
        free(res);
        freeifaddrs(addrs);
        return NULL;
    }

    // NOTE: in other places functions from libc expect an index starting from 1
    res->ifidex = option+1;

    tmp = addrs;
    while (option > 0) {
        tmp = tmp->ifa_next;
        option--;
    }

    // interface name
    int nameLen = strlen(tmp->ifa_name)+1;
    res->interfaceName = malloc(sizeof(char) * nameLen);
    if (res->interfaceName == NULL) {
        free(res);
        freeifaddrs(addrs);
        return NULL;
    }
    memset(res->interfaceName, 0, nameLen);
    memcpy(res->interfaceName, tmp->ifa_name, nameLen);
    res->interfaceName[nameLen] = 0;

    // interface mac
    struct sockaddr_ll *mac = (struct sockaddr_ll*)tmp->ifa_addr;
    memcpy(res->mac, mac->sll_addr, 8);

    // interface ip
    struct ifreq ifr;
    int interfaceNameLen = strlen(tmp->ifa_name);
    if (interfaceNameLen < sizeof(ifr.ifr_name)) {
        memcpy(ifr.ifr_name, tmp->ifa_name, interfaceNameLen);
        ifr.ifr_name[interfaceNameLen] = 0;
    } else{
        printf("Error getting interface ip, interface name too long!");
        free(res);
        freeifaddrs(addrs);
        return NULL;
    }

    // get a temp socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        printf("Error getting temp socket: ");
        perror("bind");
        free(res);
        freeifaddrs(addrs);
        return NULL;
    }

    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        perror("ioctl");
        close(fd);
        free(res);
        freeifaddrs(addrs);
        return NULL;
    }
    close(fd);

    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    res->ip = malloc(sizeof(struct in_addr));
    if (res->ip == NULL) {
        free(res);
        freeifaddrs(addrs);
        return NULL;
    }
    memcpy(res->ip, &ipaddr->sin_addr, sizeof(struct in_addr));

    freeifaddrs(addrs);
    return res;
}
