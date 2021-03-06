#include <stddef.h>
#include <stdint.h>
#include <Log.h>
#include <Idt.h>
#include <XtApi.h>
#include <Io.h>
#include <Device.h>
#include <Driver.h>
#include <Task.h>
#include <Syscalls.h>

#define TOTAL_SYSCALLS 0x400

uintptr_t syscallTable[TOTAL_SYSCALLS];

int SyscallInstall(int syscall, void* handler) {
	if (syscall > TOTAL_SYSCALLS) {
		return -1;
	}

	if (syscallTable[syscall]) {
		return -2;
	}

	syscallTable[syscall] = (uintptr_t)handler;

	return 0;
}

extern "C" {
	uint32_t SyscallHandler(int a, InterruptRegisters* regs) {
		uintptr_t p = syscallTable[regs->eax];

		if (p) {
			uint32_t(*sc)(InterruptRegisters*) = (uint32_t(*)(InterruptRegisters*)) p;
			uint32_t rv = sc(regs);
			return rv;
		}
		
		LogPrint("Attempted to call non-existant syscall %d.", regs->eax);
		return -1;
	}

	/* Syscalls */

	uint32_t ScPrintChar(InterruptRegisters* regs) {
		return XtPrintChar(regs->esi);
	}

	uint32_t ScGetRunningThreadHandle(InterruptRegisters* regs) {
		return (uint32_t)GetRunningThreadHandle();
	}

	uint32_t ScFileOpen(InterruptRegisters* regs) {
		File* f = FileOpen((char*)regs->esi);
		Handle h = CreateHandle(f);
		return h;
	}

	uint32_t ScFileClose(InterruptRegisters* regs) {
		return FileClose((File*)HandleRead(regs->esi));
	}

	uint32_t ScFileRead(InterruptRegisters* regs) {
		File* f = (File*)HandleRead(regs->esi);
		return FileReadAll(f, (uint8_t*)regs->edi);
	}

	uint32_t ScFileWrite(InterruptRegisters* regs) {
		File* f = (File*)HandleRead(regs->esi);
		return 0;
	}

	uint32_t ScFileTell(InterruptRegisters* regs) {
		File* f = (File*)HandleRead(regs->esi);
		return FileTell(f);
	}

	uint32_t ScFileSeek(InterruptRegisters* regs) {
		File* f = (File*)HandleRead(regs->ebx);
		return FileSeek(f, regs->esi, regs->edi);
	}

	uint32_t ScPrintString(InterruptRegisters* regs) {
		char* s = (char*)regs->esi;
		for (int i = 0; s[i]; i++) {
			XtPrintChar(s[i]);
		}
		return 0;
	}

	uint32_t ScAllocPages(InterruptRegisters* regs) {
		return (uint32_t)ProcAllocPages(currentProcess, (size_t)regs->esi);
	}

	uint32_t ScDebugInfo(InterruptRegisters* regs) {
		ObPrintRootDirectoryTree();
		return 0;
	}
}

void SyscallsInit() {
	SyscallInstall(0, &ScPrintChar);
	SyscallInstall(1, &ScGetRunningThreadHandle);
	SyscallInstall(2, &ScFileOpen);
	SyscallInstall(3, &ScFileClose);
	SyscallInstall(4, &ScFileRead);
	SyscallInstall(5, &ScFileWrite);
	SyscallInstall(6, &ScFileTell);
	SyscallInstall(7, &ScFileSeek);
	SyscallInstall(8, &ScPrintString);
	SyscallInstall(9, &ScAllocPages);
	SyscallInstall(10, &ScDebugInfo);
	LogPrint("SYSCALLS");
}