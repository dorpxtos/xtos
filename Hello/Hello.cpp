#include <string.h>
#include <stdio.h>
#include <Syscalls.h>
#include <Handle.h>
#include <Xt.h>

extern char badapple[];

int main(void) {
	int i = 0;
	int frame = 0;
	char c = 0;

	putc('\f');

	XtPrintString("now loading");
	Handle h = XtFileOpen("$:boot.lst");
	XtPrintString("loaded");

	while (1) {
		c = badapple[i++];

		// Decompression because of 4KiB pages.
		if (c == '2') putc('\f');
		else if (c == '3') XtPrintString("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		else if (c == '4') XtPrintString("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		else if (c == '5') XtPrintString("$$$");
		else if (c == '6') XtPrintString("               ");
		else if (c == '7') XtPrintString("            ");
		else if (c == '8') XtPrintString("      ");
		else putc(c);
		//printf("\n\n\033[36mXtOS Demo: Bad Apple [Frame %d / Char %d]\033[37m", frame, i);
	}

	return 0;
}