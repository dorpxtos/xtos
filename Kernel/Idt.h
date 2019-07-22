#pragma once
#include <stdint.h>
#include <DeclSpec.h>

#pragma pack(push,1)
struct IdtPointer {
	uint16_t limit;
	uint32_t base;
};

struct IdtEntry {
	uint16_t baseLow;
	uint16_t selector;
	uint8_t unused;
	uint8_t flags;
	uint16_t baseHigh;
};
#pragma pack(pop)

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