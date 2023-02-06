#ifndef MASKING_H_
#define MASKING_H_

#include "ip.h"

/* src ip will be changed to network address based on the mask and returned*/
IPv4* getNetworkAddress(IPv4* src, unsigned int maskLen);

/* src ip will be changed to broadcast address based on the mask and returned*/
IPv4* getBroadcastAddress(IPv4* src, unsigned int maskLen);

/* incrementing ip, 1 - ok, -1 reached broadcast address */
int incrementIp(IPv4* src, IPv4* broadcast);

#endif // !MASKING_H_
