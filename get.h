#ifndef GET_H_
#define GET_H_
// TODO: error
/* user inputs mask [0, 32] in the passed mask */
void getMask(unsigned int* mask);
/* returns ip address in dot decimal based on the hostname */
char* getIP();
/* returns which interface to use */
int getInterface();
#endif // GET_H_
