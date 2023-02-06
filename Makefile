run-dbg: comp-dbg
	sudo ./a.out
comp-dbg: main.c get.h get.c arp.h arp.c ip.h ip.c masking.h masking.c
	clang -g -Wall -Wextra main.c get.h get.c arp.h arp.c ip.h ip.c masking.h masking.c
release: main.c get.h get.c arp.h arp.c ip.h ip.c masking.h masking.c
	clang -O2 -Wall -Wextra main.c get.h get.c arp.h arp.c ip.h ip.c masking.h masking.c
	mv ./a.out ./dscvr
