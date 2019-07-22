#include <stdio.h>
#include <Syscalls.h>

#define stdout 0

int putchar(int c) {
	putc(c, stdout);
}

int puts(char* s) {
	while (*s) putchar(*s++);
	return 0;
}