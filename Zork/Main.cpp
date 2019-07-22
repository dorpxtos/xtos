#include <stddef.h>
#include <stdint.h>
#include <Handle.h>
#include <stdlib.h>
#include <Syscalls.h>
#include <Gdi.h>
#include <Printf.h>
#include <Desktop.h>

int oldmain() {
	void* mem = malloc(43243);
	printf("mem: %x", mem);

	while (1) {}
	DesktopInit();

	while (1) {
		DesktopDraw();
	}

	Handle whdl = GetDC(NULL);
	while (1) {
		for (unsigned int i = 0; i < 0xFFFFFFFF; i++) {
			GdiDrawEllipse(whdl, 400, 400, 100, 100, i);
			GdiFlush(whdl);
		}
	}
	return 0;
}