run-dbg: comp-dbg
	./a.out
comp-dbg:
	clang -g main.c -Wall -Wextra get.h get.c
