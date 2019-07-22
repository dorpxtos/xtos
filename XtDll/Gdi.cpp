#include <DeclSpec.h>
#include <Handle.h>
#include <Graphics.h>
#include <Syscalls.h>
#include <Gdi.h>

#define GDI_SYSCALL_BASE 0x100

Handle GetDC(Handle win) {
	return Syscall(GDI_SYSCALL_BASE + 0, win, 0, 0);
}

void GdiSetPixel(Handle dc, int x, int y, int color) {
	int args[] = { dc, x, y, color };
	Syscall(GDI_SYSCALL_BASE + 1, args, 0, 0);
}

void GdiDrawRect(Handle dc, Rectangle* rect, Color color) {
	Syscall(GDI_SYSCALL_BASE + 2, dc, rect, color);
}

void GdiBrushRect(Handle dc, Rectangle* rect, Handle brush) {
	Syscall(GDI_SYSCALL_BASE + 3, dc, rect, brush);
}

void GdiDrawRectOutline(Handle dc, int x, int y, int w, int h, int color) {
	int args[] = { dc, x, y, w, h, color };
	Syscall(GDI_SYSCALL_BASE + 4, args, 0, 0);
}

void GdiDrawLine(Handle dc, int x1, int y1, int x2, int y2, int color) {
	int args[] = { dc, x1, y1, x2, y2, color };
	Syscall(GDI_SYSCALL_BASE + 5, args, 0, 0);
}

void GdiDrawEllipse(Handle dc, int xc, int yc, int rx, int ry, int color) {
	int args[] = { dc, xc, yc, rx, ry, color };
	Syscall(GDI_SYSCALL_BASE + 6, args, 0, 0);
}

Handle GdiCreateSolidBrush(Color color) {
	return Syscall(GDI_SYSCALL_BASE + 7, color, 0, 0);
}

Handle GdiCreateHatchedBrush(Color color, Hatch hatch) {
	return Syscall(GDI_SYSCALL_BASE + 8, color, hatch, 0);
}

void GdiFlush(Handle dc) {
	return Syscall(GDI_SYSCALL_BASE + 9, dc, 0, 0);
}