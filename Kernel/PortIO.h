#pragma once
#include <stdint.h>

inline void outb(uint16_t port, uint8_t val) {
	_asm {
		mov dx, [port]
		mov al, [val]
		out dx, al
	}
}

inline uint8_t inb(uint16_t port) {
	uint8_t returnValue;
	_asm {
		mov dx, [port]
		in al, dx
		mov byte ptr [returnValue], al
	}
	return returnValue;
}

inline void outw(uint16_t port, uint16_t val) {
	_asm {
		mov dx, [port]
		mov ax, [val]
		out dx, ax
	}
}

inline uint16_t inw(uint16_t port) {
	uint16_t returnValue;
	_asm {
		mov dx, [port]
		in ax, dx
		mov word ptr[returnValue], ax
	}
	return returnValue;
}

inline void outl(uint16_t port, uint32_t val) {
	_asm {
		mov dx, [port]
		mov eax, [val]
		out dx, eax
	}
}

inline uint32_t inl(uint16_t port) {
	uint16_t returnValue;
	_asm {
		mov dx, [port]
		in eax, dx
		mov dword ptr[returnValue], eax
	}
	return returnValue;
}