run-dbg: comp-dbg
	sudo ./a.out
comp-dbg:
	clang -g main.c -Wall -Wextra get.h get.c arp.h arp.c
