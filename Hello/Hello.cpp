#include <string.h>
#include <stdio.h>
#include <Syscalls.h>
#include <Gdi.h>
#include <Handle.h>
#include <Xt.h>

extern char badapple[];

int pxx = 0;
int pxy = 0;

Handle dc;
Handle brush;
Handle nobrush;
Rectangle r;

void pxchar(char c, int n) {
	if (c == '\n') {
		pxx = 0;
		pxy++;
		return;
	} else if (c == '\f') {
		pxx = 0;
		pxy = 0;
		GdiFlush(dc);
		return;
	}
	else {
		r.height = 5;
		r.width = 5 * n;
		r.x = pxx * 5;
		r.y = pxy * 5;

		if (c == '$') {
			GdiDrawRect(dc, &r, brush);
		} else {
			GdiDrawRect(dc, &r, nobrush);
		}

		pxx+=n;
	}
}

void pxstring(char* s) {
	pxchar(s[0], strlen(s));
}

int main(void) {
	int i = 0;
	int frame = 0;
	char c = 0;

	//XtPrintString("The quick brown fox");
	for (int q = 0; q < 9999999; q++);
	//XtPrintString("CREATED dc");
	dc = GetDC(NULL);
	nobrush = GdiCreateSolidBrush(0);
	brush = GdiCreateSolidBrush(0x1BADB002);

	//XtPrintString("The quick brown fox 2");

	while (1) {
		c = badapple[i++];

		// Decompression because of 4KiB pages.
		if (c == '2') pxstring("\f");
		else if (c == '3') pxstring("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		else if (c == '4') pxstring("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
		else if (c == '5') pxstring("$$$");
		else if (c == '6') pxstring("               ");
		else if (c == '7') pxstring("            ");
		else if (c == '8') pxstring("      ");
		else pxchar(c, 1);
		//printf("\n\n\033[36mXtOS Demo: Bad Apple [Frame %d / Char %d]\033[37m", frame, i);
	}

	return 0;
}