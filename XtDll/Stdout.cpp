#include <stdio.h>
#include <Syscalls.h>

int putc(int c) {
	XtPrintChar(c);
	return c;
}

int puts(char* s) {
	while (*s) putc(*s++);
	return 0;
}