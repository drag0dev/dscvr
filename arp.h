#ifndef ARP_H_
#define ARP_H_

#include <stdint.h>

int bind_arp(int* fd, int ifidx);
int send_arp(int fd, char* ip, int ifidx);

#endif // ARP_H_
