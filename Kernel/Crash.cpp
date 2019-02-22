#include <TextMode.h>
#include <Log.h>
#include <Idt.h>
#include <Crash.h>

void CrashCritical(char* message, InterruptRegisters* registers) {
	uint32_t _cr2;
	uint32_t _cr3;
	_asm {
		mov eax, cr2
		mov _cr2, eax
		mov eax, cr3
		mov _cr3, eax
	}
	LogPrint("\nCRITICAL ERROR LOL!\n%s\n\nAX: %80X BX: %80X CX: %80X DX: %80X SI: %80X DI: %80X\nBP: %80X SP: %80X CS: %80X SS: %80X IP: %80X US: %80X\nEC: %80X C2: %80X C3: %80X\n", message, registers->eax, registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi, registers->ebp, registers->esp, registers->cs, registers->ss, registers->eip, registers->useresp, registers->err_code, _cr2, _cr3);

	_asm {
		cli
		hlt
	}
}