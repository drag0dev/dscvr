#ifndef GET_H_
#define GET_H_
// TODO: error

#include <netinet/in.h>

/* struct used for passing information around */
typedef struct ifInfo {
    unsigned char mac[8];
    char* interfaceName;
    struct in_addr* ip;
    int ifidex;
} interfaceInfo;

/* user inputs mask [0, 31] in the passed mask */
/* nothing to find on a subnet with a mask 32 */
void getMask(unsigned int* mask);

/* returns which interface to use and its info*/
interfaceInfo* getInterface();

#endif // GET_H_
