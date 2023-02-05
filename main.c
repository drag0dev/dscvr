#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "get.h"
#include "arp.h"
#include "ip.h"
#include "masking.h"

// TODO: cant know which interface we want, list all ips and interfacesa and ask user which one to use
// TODO: based on the interface can get default mask, and then confirm with user, if not user can enter

int main () {
    interfaceInfo* ifInfo = getInterface();
    if (ifInfo == NULL) return -1;

    int fd;
    bind_arp(&fd, ifInfo->ifidex);
    if (fd == -1) {
        free(ifInfo);
        return -1;
    }

    unsigned int mask;
    getMask(&mask);

    IPv4* currentIp = malloc(sizeof(IPv4));
    if (currentIp == NULL) {
        printf("No memory for the current IP!");
        return -1;
    }

    char* tempIpStr = inet_ntoa(*(ifInfo->ip));
    iptoo(currentIp, tempIpStr, strlen(tempIpStr));

    IPv4* ip = getNetworkAddress(currentIp, mask);
    if (ip == NULL) {
        printf("No memory for the network address!");
        return -1;
    }

    IPv4* broadcast = getBroadcastAddress(currentIp, mask);
    if (broadcast == NULL) {
        printf("No memory for the broadcast address!");
        return -1;
    }
    free(currentIp);

    pthread_t threadId[2];
    void* senderArgs[4] = {ip, broadcast, ifInfo, &fd};
    pthread_create(threadId, NULL, sender, senderArgs);

    printf("Received: \n");
    pthread_create(&threadId[1], NULL, receiver, &fd);

    pthread_join(threadId[0], NULL);
    pthread_join(threadId[1], NULL);

    close(fd);
    return 0;
}
