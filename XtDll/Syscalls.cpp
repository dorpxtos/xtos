#include <Handle.h>
#include <Syscalls.h>

void XtPrintChar(int c) {
	_asm {
		mov eax, 0
		mov esi, c
		int 128
	}
}

Handle XtGetRunningThreadHandle() {
	Handle rv;
	_asm {
		mov eax, 1
		int 128
		mov rv, eax
	}
	return rv;
}

Handle XtFileOpen(char* name) {
	Handle rv;
	_asm {
		mov eax, 2
		mov esi, name
		int 128
		mov rv, eax
	}
	return rv;
}

int XtFileRead(Handle f, uint8_t* buffer, int count) {
	Handle rv;
	_asm {
		mov eax, 3
		mov esi, f
		mov edi, buffer
		mov ecx, count
		int 128
		mov rv, eax
	}
	return rv;
}

int XtFileWrite(Handle f, uint8_t* buffer, int count) {
	Handle rv;
	_asm {
		mov eax, 4
		mov esi, f
		mov edi, buffer
		mov ecx, count
		int 128
		mov rv, eax
	}
	return rv;
}

void XtPrintString(char* msg) {
	_asm {
		mov eax, 5
		mov esi, msg
		int 128
	}
}