#include <stddef.h>
#include <stdint.h>
#include <Vbe.h>

int VbeSetMode(uint8_t mode) {
	return 0;
}

int VbeInit() {
	VbeSetMode(0);
	return 0;
}

int DllMain(short a, int b) {
	return VbeInit();
}