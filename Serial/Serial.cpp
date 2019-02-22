#include <PortIO.h>
#include <DeclSpec.h>
#include <Serial.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

int isReady = 0;

DllExport int SerialPrintChar(char c) {
	while (!(inb(COM1 + 5) & 0x20));
	outb(COM1, c);
	return c;
}

DllExport int SerialPrintString(char* c) {
	while (*c) {
		int r = SerialPrintChar(*c++);
		if (r) return r;
	}

	return 0;
}

DllExport int SerialIsReady() {
	return isReady;
}

DllExport int SerialInit() {
	outb(COM1 + 1, 0x00);
	outb(COM1 + 3, 0x80);
	outb(COM1 + 0, 0x03);
	outb(COM1 + 1, 0x00);
	outb(COM1 + 3, 0x03);
	outb(COM1 + 2, 0xC7);
	outb(COM1 + 4, 0x0B);

	isReady = 1;
	return 0;
}

int DllMain(short a, int b) {
	return SerialInit();
}
