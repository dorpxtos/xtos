#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <Paging.h>
#include <PortableExecutable.h>
#include <Idt.h>
#include <Pic.h>
#include <ObjectManager.h>
#include <Handle.h>
#include <Stacktrace.h>
#include <Task.h>

#define MAX_TOTAL_PROCESSES 4096
#define MAX_TOTAL_THREADS 4096
#define STACK_SIZE 4096

Process** processes;
Thread** threads;

Process* currentProcess;
Thread* currentThread;

bool schedulerEnabled = false;
bool schedulerWarmedUp = false;
int yieldInterval = 1;

extern "C" void TaskSpinup(TaskRegisters*, uintptr_t);

static Process* GetProcessFromThread(Thread* t) {
	if (t->process > MAX_TOTAL_PROCESSES) {
		Log("!! Too large proc # !!");
		_asm int 3
	}
	return processes[t->process];
}

int TaskCreateStack(Thread* t) {
	Process* proc = GetProcessFromThread(t);
	int stksize = proc->pe->header->sizeOfStackCommit;
	int pages = stksize / PAGE_SIZE;
	size_t stack = (size_t)PmmAlloc(pages);
	size_t vstack = proc->stackTop;
	proc->stackTop += stksize;
	MapPage(proc->pagemap, stack, vstack, 0x7);
	MapPage(kernelPagemap, stack, vstack, 0x7);
	t->ustack = vstack;
	t->registers.esp = vstack;

	return 0;
}

Obj* TaskObThreadCreate(int n, Thread** thread) {
	char* oname = (char*)MemoryAllocate(32);
	strcpy(oname, "Threads/");
	itoa(n, oname + strlen("Threads/"), 10);
	Obj* o = ObCreate(oname, "Thread", sizeof(Thread), (void**)&threads[n]);
	threads[n]->id = n;
	*thread = threads[n];
	return o;
}

Obj* TaskObProcessCreate(int n, Process** process) {
	char* oname = (char*)MemoryAllocate(32);
	strcpy(oname, "Processes/");
	itoa(n, oname + strlen("Processes/"), 10);
	Obj* o = ObCreate(oname, "Process", sizeof(Process), (void**)&processes[n]);
	processes[n]->id = n;
	*process = processes[n];
	return o;
}

Thread* TaskNewThread(Process* proc) {
	for (int i = 0; i < MAX_THREADS; i++) {
		if (!proc->threads[i]) {
			for (int j = 0; j < MAX_TOTAL_THREADS; j++) {
				if (!threads[j]) {
					TaskObThreadCreate(j, &proc->threads[i]);
					break;
				}
			}

			if (proc->threads[i]) {
				proc->threads[i]->active = true;
				proc->threads[i]->process = proc->id;

				memset(&proc->threads[i]->registers, 0, sizeof(InterruptRegisters));
				proc->threads[i]->registers.eip = proc->pe->header->imageBase + proc->pe->header->addressOfEntryPoint;
				proc->threads[i]->registers.cs = 0x1b;
				proc->threads[i]->registers.ds = 0x23;
				proc->threads[i]->registers.es = 0x23;
				proc->threads[i]->registers.fs = 0x23;
				proc->threads[i]->registers.gs = 0x23;
				proc->threads[i]->registers.ss = 0x23;
				proc->threads[i]->registers.eflags = 0x202;

				TaskCreateStack(proc->threads[i]);

				return proc->threads[i];
			}
		}
	}

	return 0;
}

Process* TaskNewProcess(char* path) {
	Log("Creating new process: '%s'", path);
	Process* proc;

	for (int i = 0; i < MAX_TOTAL_PROCESSES; i++) {
		if (!processes[i]) {
			TaskObProcessCreate(i, &proc);
			break;
		}
	}

	if (proc) {
		memset(proc->handles, 0, MAX_HANDLES * sizeof(Handle));

		proc->pe = PeLoad(path, NULL, CreatePagemap());
		proc->name = path;
		proc->pagemap = proc->pe->pagemap;
		proc->stackTop = proc->pe->top;

		Thread* adamThread = TaskNewThread(proc);

		Log("Created process '%s' (PID: %d) (TID: %d) (Pagemap: %x)", proc->name, proc->id, adamThread->id, proc->pagemap);

		return proc;
	} else {
		return NULL;
	}
}

Handle TaskProcessCreateHandle(Process* proc, void* obj) {
	for (int i = 0; i < MAX_HANDLES; i++) {
		if (!proc->handles[i]) {
			proc->handles[i] = (uintptr_t)obj;
			/*Obj* objh = ObGetHeader(obj);
			objh->pointerCount++;*/
			return i;
		}
	}

	Log("!! Handle table filled !!");
	return NULL;
}

Handle CreateHandle(void* obj) {
	Handle h = TaskProcessCreateHandle(currentProcess, obj);
	return h;
}

void* HandleRead(Handle h) {
	return (void*)(currentProcess->handles[h]);
}

Handle GetRunningThreadHandle() {
	return CreateHandle(currentThread);
}

void TaskYield(InterruptRegisters* oldRegs) {
	//Log("Yield from '%s' (PID: %d) (TID: %d)", currentProcess->name, currentProcess->id, currentThread->id);
	//Log("ss=%x esp=%x sizeof=%x", oldRegs->ss, oldRegs->esp, sizeof(TaskRegisters));

	if (currentProcess->id > MAX_TOTAL_PROCESSES || currentThread->id > MAX_TOTAL_THREADS) {
		Log("!! CORRUPTION IN SCHEDULER !!");
		_asm int 3;
	}

	if (schedulerWarmedUp) {
		currentThread->registers.eax = oldRegs->eax;
		currentThread->registers.ebx = oldRegs->ebx;
		currentThread->registers.ecx = oldRegs->ecx;
		currentThread->registers.edx = oldRegs->edx;
		currentThread->registers.esi = oldRegs->esi;
		currentThread->registers.edi = oldRegs->edi;
		currentThread->registers.ebp = oldRegs->ebp;
		currentThread->registers.esp = oldRegs->useresp;
		currentThread->registers.eip = oldRegs->eip;
		currentThread->registers.eflags = oldRegs->eflags;
	} else schedulerWarmedUp = true;

	int newThreadId;
	for (newThreadId = currentThread->id + 1; newThreadId != currentThread->id; newThreadId++) {
		if (newThreadId >= MAX_TOTAL_THREADS) {
			newThreadId = 0;
		}
		if (threads[newThreadId]) {
			currentThread = threads[newThreadId];
			currentProcess = GetProcessFromThread(currentThread);
			break;
		}
	}

	uintptr_t cr3 = (uintptr_t)currentProcess->pagemap;
	//Log("Yield to '%s' (PID: %d) (TID: %d) cr3=%x eip=%x", currentProcess->name, currentProcess->id, currentThread->id, cr3, currentThread->registers.eip);

	//Stacktrace(5);

	PicSendEoi((uint8_t)oldRegs->int_no);
	
	TaskSpinup(&currentThread->registers, cr3);
}

void* ProcAllocPages(Process* proc, size_t pages) {
	if (!proc) {
		return NULL;
	}

	if (!pages) {
		return NULL;
	}

	void* p = PmmAlloc(pages);

	if (!p) {
		return NULL;
	}

	void* ptop = (void*)proc->stackTop;

	for (int i = 0; i < pages; i++) {
		MapPage(proc->pagemap, (size_t)p, proc->stackTop, 0x7);
		MapPage(kernelPagemap, (size_t)p, proc->stackTop, 0x7);
		proc->stackTop += PAGE_SIZE;
	}

	return ptop;
}

void TaskInit() {
	processes = (Process**)MemoryAllocate(MAX_TOTAL_PROCESSES * sizeof(Process*));
	threads = (Thread**)MemoryAllocate(MAX_TOTAL_THREADS * sizeof(Thread*));

	Log("TASK");
}