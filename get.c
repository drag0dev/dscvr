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
