run-dbg: comp-dbg
	sudo ./a.out
comp-dbg: main.c get.h get.c arp.h arp.c ip.h ip.c
	clang -g -Wall -Wextra main.c get.h get.c arp.h arp.c ip.h ip.
