#pragma once
#include <stdint.h>
#include <DeclSpec.h>

struct InterruptRegisters {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax, int_no, err_code, eip, cs, eflags, useresp, ss;
};

inline void DisableInterrupts() {
	_asm cli;
}

inline void EnableInterrupts() {
	_asm sti;
}

void IdtInit();


DllExport int IrqInstall(uint8_t, void(*)(InterruptRegisters*));
DllExport int IrqUninstall(uint8_t);