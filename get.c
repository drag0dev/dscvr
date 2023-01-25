#include "get.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char* getIP() {
    char hostbuffer[256];
    char* IPBuffer;
    int hostname;
    struct hostent* host_entry;
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1) {
        printf("Error getting hostname: ");
        perror("gethostname");
        exit(-1);
    }

    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) {
        printf("Error getting host by name: ");
        perror("gethostbyname");
        exit(-1);
    }

    IPBuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[1]));
    if (IPBuffer == NULL){
        printf("Error convert to dot-dec format: ");
        perror("inet_ntoa");
        exit(-1);
    }
    return IPBuffer;
}

void getMask(unsigned int* mask) {
    while (1) {
        // get user input
        printf("Enter mask (0-32): ");
        scanf("%u", mask);

        // clear stdin
        int c;
        do {
            c = getchar();
        } while (c != '\n' && c != EOF);

        if ((*mask) >= 0 && (*mask) <= 32) break; // mask can only be [0, 32]
    }
}

int getInterface() {
    struct ifaddrs *addrs, *tmp;
    if (getifaddrs(&addrs) != 0) {
        perror("getifaddrs");
        return -1;
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
    printf("I: %d\n", numberOfInterfaces);
    while (1) {
        printf("Choose an interface: ");
        scanf("%d", &option);
        if (option >= 0 && option < numberOfInterfaces) break;
    }

    freeifaddrs(addrs);
    return option;
}
