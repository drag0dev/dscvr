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

// small mem leaks in case of early returns are left for sake of readability
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
        close(fd);
        return -1;
    }

    char* tempIpStr = inet_ntoa(*(ifInfo->ip));
    iptoo(currentIp, tempIpStr, strlen(tempIpStr));

    IPv4* ip = getNetworkAddress(currentIp, mask);
    if (ip == NULL) {
        printf("No memory for the network address!");
        close(fd);
        return -1;
    }

    IPv4* broadcast = getBroadcastAddress(currentIp, mask);
    if (broadcast == NULL) {
        printf("No memory for the broadcast address!");
        close(fd);
        return -1;
    }
    free(currentIp);

    pthread_t threadId[2];
    void* senderArgs[4] = {ip, broadcast, ifInfo, &fd};
    pthread_create(threadId, NULL, sender, senderArgs);

    printf("Waiting for responses indefinitely, CTRL+C to stop\n");
    printf("Received: \n");
    pthread_create(&threadId[1], NULL, receiver, &fd);

    pthread_join(threadId[0], NULL);
    pthread_join(threadId[1], NULL);

    close(fd);
    return 0;
}
