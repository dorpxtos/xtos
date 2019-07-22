#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Log.h>
#include <Handle.h>
#include <Task.h>
#include <Idt.h>
#include <Device.h>
#include <Driver.h>
#include <MemoryAllocator.h>
#include <ObjectManager.h>
#include <Video.h>
#include <Syscalls.h>
#include <Id.h>
#include <Status.h>
#include <Gdi.h>
#include <User.h>

#define USER_SYSCALL_BASE 0x200
#define UserSyscallInstall(x, y) SyscallInstall(USER_SYSCALL_BASE + x, &y)

DriverObject* windowDriver;

Obj* WindowObCreate(Window** win) {
	char oname[256];
	memset(oname, 0, 256);
	strcpy(oname, "Windows/");
	IdGenerate(oname + strlen(oname));
	Obj* o = ObCreate(strdup(oname), "Window", sizeof(Window), (void**)win);
	return o;
}

Handle UserCreateWindow(char* name, int x, int y, int w, int h) {
	Window* win;
	char dn[100];
	strcpy(dn, "Window");
	IdGenerate(dn + strlen(dn));
	WindowObCreate(&win);
	win->title = strdup(name);
	win->x = x;
	win->y = y;
	win->width = w;
	win->height = h;
	win->display = mainDisplay;
	win->displayDevice = DisplayObCreate(strdup(dn), w, h, 24, (uintptr_t)MemoryAllocate(win->height * win->width * (24 / 8)), windowDriver, &win->windisp);
	Handle rwhdl = CreateHandle(win);
	return rwhdl;
}

bool UserShowWindow(Handle whdl, ShowMode showmode) {
	Window* w = (Window*)HandleRead(whdl);
	bool prevshownstatus = (w->showMode == SHOW);
	w->showMode = showmode;
	return prevshownstatus;
}

Handle UserWindowGetDisplay(Handle whdl) {
	Window* w = (Window*)HandleRead(whdl);
	if (!w) return STATUS_NULLHANDLE;
	return CreateHandle(w->windisp);
}

Status UserDrawWindow(Handle whdl) {
	Window* w = (Window*)HandleRead(whdl);
	if (!w) return STATUS_NULLHANDLE;
	if (w->showMode == SHOW) {
		for (int y = 0; y < w->height; y++) {
			for (int x = 0; x < w->width; x++) {
				VideoPreBufferSetPixel(w->display, w->x + x, w->y + y, w->windisp->framebuffer[y * w->width + x]);
			}
		}
	}
	return STATUS_SUCCESS;
}

uint32_t UserSetWindowAttribute(Handle whdl, int attr, uint32_t value) {
	Window* w = (Window*)HandleRead(whdl);
	if (!w) return -1;
	uint32_t p = w->attributes[attr];
	w->attributes[attr] = value;
	return p;
}

uint32_t UserGetWindowAttribute(Handle whdl, int attr) {
	Window* w = (Window*)HandleRead(whdl);
	if (!w) return -1;
	return w->attributes[attr];
}

uint32_t ScUserCreateWindow(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	return UserCreateWindow((char*)args[0], args[1], args[2], args[3], args[4]);
}

uint32_t ScUserShowWindow(InterruptRegisters* regs) {
	return UserShowWindow(regs->ebx, (ShowMode)regs->esi);
}

uint32_t ScUserWindowGetDisplay(InterruptRegisters* regs) {
	return UserWindowGetDisplay(regs->ebx);
}

uint32_t ScUserDrawWindow(InterruptRegisters* regs) {
	return UserDrawWindow(regs->ebx);
}

uint32_t ScUserSetWindowAttribute(InterruptRegisters* regs) {
	return UserSetWindowAttribute((Handle)regs->ebx, (int)regs->esi, (uint32_t)regs->edi);
}

uint32_t ScUserGetWindowAttribute(InterruptRegisters* regs) {
	return UserGetWindowAttribute((Handle)regs->ebx, (int)regs->esi);
}

void UserKrnlInit() {
	DriverObCreate("Window", "Window Display Driver", &windowDriver);
	UserSyscallInstall(0, ScUserCreateWindow);
	UserSyscallInstall(1, ScUserShowWindow);
	UserSyscallInstall(2, ScUserWindowGetDisplay);
	UserSyscallInstall(3, ScUserDrawWindow);
	UserSyscallInstall(4, ScUserSetWindowAttribute);
	UserSyscallInstall(5, ScUserGetWindowAttribute);
	Log("User Kernel initialized");
}