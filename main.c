#include "get.h"

int main () {
    char* ip = getIP();
    unsigned int mask;
    getMask(&mask);
    return 0;
}
