#include <stdbool.h>
#include <Handle.h>
#include <Syscalls.h>
#include <User.h>

#define USER_SYSCALL_BASE 0x200

Handle CreateWindow(char* name, int x, int y, int h, int w) {
	int args[] = { name, x, y, h, w };
	return Syscall(USER_SYSCALL_BASE + 0, args, 0, 0);
}

bool ShowWindow(Handle whdl, ShowMode showmode) {
	return Syscall(USER_SYSCALL_BASE + 1, whdl, showmode, 0);
}

Handle WindowGetDisplay(Handle whdl) {
	return Syscall(USER_SYSCALL_BASE + 2, whdl, 0, 0);
}

void DrawWindow(Handle whdl) {
	Syscall(USER_SYSCALL_BASE + 3, whdl, 0, 0);
}

uint32_t SetWindowAttribute(Handle whdl, int attr, uint32_t value) {
	Syscall(USER_SYSCALL_BASE + 4, whdl, attr, value);
}

uint32_t GetWindowAttribute(Handle whdl, int attr) {
	Syscall(USER_SYSCALL_BASE + 5, whdl, attr, 0);
}