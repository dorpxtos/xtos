#include <TextMode.h>
#include <Log.h>
#include <Idt.h>
#include <Gdt.h>
#include <Task.h>
#include <Stacktrace.h>
#include <Crash.h>

#define CRASH_STACKTRACE_LENGTH 30

void CrashCritical(char* message, InterruptRegisters* registers) {
	uint32_t _cr0;
	uint32_t _cr2;
	uint32_t _cr3;
	uint32_t _dr0;
	uint32_t _dr1;
	uint32_t _dr2;
	uint32_t _dr3;
	uint32_t _dr6;
	uint32_t _dr7;
	uint16_t tr;
	GdtPointer gdtr;
	IdtPointer idtr;

	_asm {
		mov eax, cr2
		mov _cr2, eax
		mov eax, cr3
		mov _cr3, eax
		mov eax, cr0
		mov _cr0, eax
		mov eax, dr0
		mov _dr0, eax
		mov eax, dr1
		mov _dr1, eax
		mov eax, dr2
		mov _dr2, eax
		mov eax, dr3
		mov _dr3, eax
		mov eax, dr6
		mov _dr6, eax
		mov eax, dr7
		mov _dr7, eax
		str tr
		sgdt gdtr
		sidt idtr
	}

	LogPrint(
		"\nCRITICAL ERROR LOL!\n"
		"%s (%20X,%20X)\n\n"
		"AX =%80X BX=%80X CX=%80X DX=%80X SI=%80X DI=%80X BP=%80X SP=%80X\n"
		"CS =%80X SS=%80X TR=%80X\n"
		"IP =%80X FL=%80X US=%80X\n"
		"C0 =%80X C2=%80X C3=%80X\n"
		"D0 =%80X D1=%80X D2=%80X D3=%80X D6=%80X D7=%80X\n"
		"GDT=%80X %80X\nIDT=%80X %80X\n\n",
		message,
		registers->int_no, registers->err_code,
		registers->eax, registers->ebx, registers->ecx, registers->edx, registers->esi, registers->edi, registers->ebp, registers->esp,
		registers->cs, registers->ss, tr,
		registers->eip, registers->eflags, registers->useresp,
		_cr0, _cr2, _cr3,
		_dr0, _dr1, _dr2, _dr3, _dr6, _dr7,
		gdtr.offset, gdtr.size,
		idtr.base, idtr.limit);

	Stacktrace(CRASH_STACKTRACE_LENGTH);

	_asm {
		cli
		hlt
	}
}