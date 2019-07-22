#pragma once
#include <stddef.h>
#include <stdint.h>

inline void CLI() {
	_asm {
		cli
	}
}

inline void STI() {
	_asm {
		sti
	}
}

inline void HLT() {
	_asm {
		hlt
	}
}

inline void SUPERHANG() {
l:
	_asm {
		cli
		hlt
		jmp l
	}
}

inline void LGDT(void* m32) {
	_asm {
		lgdt m32
	}
}

inline void LIDT(void* m32) {
	_asm {
		lidt m32
	}
}

inline void SGDT(void* m32) {
	_asm {
		sgdt m32
	}
}

inline void SIDT(void* m32) {
	_asm {
		sidt m32
	}
}

inline void JMP(uintptr_t addr32) {
	_asm {
		jmp addr32
	}
}