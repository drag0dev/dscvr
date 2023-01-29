#ifndef MASKING_H_
#define MASKING_H_

#include "ip.h"

/* src ip will be changed to network address based on the mask and returned*/
IPv4* getNetworkAddress(IPv4* src, unsigned int mask);

// TODO: do we really need this
/* src ip will be changed in place to broadcast address based on the mask*/
void getBroadcastAddress(IPv4* src, unsigned int mask);

/* incrementing ip, 0 - ok, -1 reached broadcast address */
int incrementIp(IPv4* src, unsigned int mask);

#endif // !MASKING_H_
