#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <DeclSpec.h>
#include <Handle.h>
#include <Graphics.h>
#include <Syscalls.h>

void XtPrintChar(int c) {
	Syscall(0, 0, c, 0);
}

Handle XtGetRunningThreadHandle() {
	return Syscall(1, 0, 0, 0);
}

Handle XtFileOpen(char* name) {
	return Syscall(2, 0, (uint32_t)name, 0);
}

Handle XtFileClose(Handle f) {
	return Syscall(3, 0, (uint32_t)f, 0);
}

int XtFileRead(Handle f, uint8_t* buffer, int count) {
	return Syscall(4, count, (uint32_t)f, (uint32_t)buffer);
}

int XtFileWrite(Handle f, uint8_t* buffer, int count) {
	return Syscall(5, count, (uint32_t)f, (uint32_t)buffer);
}

int XtFileTell(Handle f) {
	return Syscall(6, 0, (uint32_t)f, 0);
}

off_t XtFileSeek(Handle f, off_t offset, int whence) {
	return Syscall(7, (uint32_t)f, (uint32_t)offset, (uint32_t)whence);
}

void XtPrintString(char* msg) {
	Syscall(8, 0, (uint32_t)msg, 0);
}

void* XtAllocPages(size_t pages) {
	return (void*)Syscall(9, 0, (uint32_t)pages, 0);
}