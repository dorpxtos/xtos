#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <Paging.h>
#include <Idt.h>
#include <PortableExecutable.h>
#include <Handle.h>

#define MAX_THREADS 32
#define MAX_HANDLES 512

typedef uint32_t tid_t;
typedef uint32_t pid_t;

struct TaskRegisters {
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t eip;
	uint32_t cs;
	uint32_t ds;
	uint32_t es;
	uint32_t fs;
	uint32_t gs;
	uint32_t ss;
	uint32_t eflags;
};

struct Thread {
	tid_t id;
	pid_t process;
	bool active;
	uintptr_t ustack;
	uintptr_t kstack;
	TaskRegisters registers;
};

struct Process {
	pid_t id;
	bool active;
	Thread* threads[MAX_THREADS];
	char* name;
	PeProgram* pe;
	PageTableEntry* pagemap;
	uintptr_t stackTop;
	uintptr_t handles[MAX_HANDLES];
};

extern bool schedulerEnabled;
extern int yieldInterval;
extern Process* currentProcess;
extern Thread* currentThread;

DllExport Process* TaskNewProcess(char*);
DllExport Thread* TaskNewThread(Process*);
DllExport Handle CreateHandle(void*);
DllExport Handle GetRunningThreadHandle();
DllExport void* HandleRead(Handle);
DllExport void* ProcAllocPages(Process* proc, size_t pages);
void TaskYield(InterruptRegisters*);
void TaskInit();
