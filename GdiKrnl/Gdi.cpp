#include <stdbool.h>
#include <string.h>
#include <DeclSpec.h>
#include <Idt.h>
#include <Syscalls.h>
#include <ObjectManager.h>
#include <Task.h>
#include <Device.h>
#include <Video.h>
#include <Log.h>
#include <Id.h>
#include <ObjectManager.h>
#include <Graphics.h>
#include <User.h>
#include <Gdi.h>

#define MAX_HATCH_FUNCTIONS 25
#define GDI_SYSCALL_BASE 0x100
#define GdiSyscallInstall(x, y) SyscallInstall(GDI_SYSCALL_BASE + x, &y)

DisplayObject* mainDisplay;

uintptr_t hatchFunctions[MAX_HATCH_FUNCTIONS];

Obj* BrushObCreate(BrushStyle style, Color color, Hatch hatch, Brush** brush) {
	char oname[256];
	memset(oname, 0, 256);
	strcpy(oname, "Brushes/");
	IdGenerate(oname + strlen(oname));
	Obj* obj = ObCreate(strdup(oname), "Brush", sizeof(Brush), (void**)brush);
	(*brush)->style = style;
	(*brush)->color = color;
	(*brush)->hatch = hatch;
	return obj;
}

bool HatchHorizontally(int x, int y) {
	return (x % 2);
}

bool HatchVertically(int x, int y) {
	return (y % 2);
}

bool HatchHorizVert(int x, int y) {
	return (x % 2) && (y % 2);
}

bool HatchCheckerboard(int x, int y) {
	return (x + y) % 2;
}

Handle GetDC(Handle win) {
	Handle dc;

	if (win) {
		dc = CreateHandle(((Window*)HandleRead(win))->displayDevice);
	} else {
		dc = CreateHandle(ObSuper(mainDisplay, DeviceObject));
	}

	return dc;
}

void GdiSetPixel(Handle dc, int x, int y, int color) {
	DisplayObject* d = (DisplayObject*)ObSub(HandleRead(dc), DeviceObject, DisplayObject);
	if (!d || x > d->width || y > d->height) return;
	VideoPreBufferSetPixel(d, x, y, color);
}

void GdiBrushPixel(Handle dc, int x, int y, Handle bhandle) {
	Brush* brush = (Brush*)HandleRead(bhandle);
	if (brush->style == SOLID) {
		GdiSetPixel(dc, x, y, brush->color);
		return;
	}

	if (hatchFunctions[brush->hatch]) {
		bool(*fn)(int, int) = (bool(*)(int, int))hatchFunctions[brush->hatch];
		if (fn(x, y)) {
			GdiSetPixel(dc, x, y, brush->color);
		}
	}
}

void GdiDrawRect(Handle dc, Rectangle* rect, Color color) {
	for (int i = rect->y; i <= rect->y + rect->height; i++) {
		for (int j = rect->x; j <= rect->x + rect->width; j++) {
			GdiSetPixel(dc, j, i, color);
		}
	}
}

void GdiBrushRect(Handle dc, Rectangle* rect, Handle bhandle) {
	for (int i = rect->y; i <= rect->y + rect->height; i++) {
		for (int j = rect->x; j <= rect->x + rect->width; j++) {
			GdiBrushPixel(dc, j, i, bhandle);
		}
	}
}


void GdiDrawRectOutline(Handle dc, int x, int y, int w, int h, Color color) {
	for (int i = y; i <= y + h; i++) {
		for (int j = x; j <= x + w; j++) {
			if (i == y || i == y + h || j == x || j == x + w) {
				GdiSetPixel(dc, j, i, color);
			}
		}
	}
}

void GdiBrushRectOutline(Handle dc, int x, int y, int w, int h, Handle bhandle) {
	for (int i = y; i <= y + h; i++) {
		for (int j = x; j <= x + w; j++) {
			if (i == y || i == y + h || j == x || j == x + w) {
				GdiBrushPixel(dc, j, i, bhandle);
			}
		}
	}
}

void GdiDrawEllipse(Handle dc, int xc, int yc, int rx, int ry, Color color) {
	int hh = ry * ry;
	int ww = rx * rx;
	int hhww = hh * ww;
	int x0 = rx;
	int dx = 0;

	// do the horizontal diameter
	for (int x = -rx; x <= rx; x++) GdiSetPixel(dc, xc + x, yc, color);

	// now do both halves at the same time, away from the diameter
	for (int y = 1; y <= ry; y++) {
		int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
		for (; x1 > 0; x1--) if (x1*x1*hh + y*y*ww <= hhww) break;
		dx = x0 - x1;  // current approximation of the slope
		x0 = x1;

		for (int x = -x0; x <= x0; x++) {
			GdiSetPixel(dc, xc + x, yc - y, color);
			GdiSetPixel(dc, xc + x, yc + y, color);
		}
	}
}

void GdiBrushEllipse(Handle dc, int xc, int yc, int rx, int ry, Handle bhandle) {
	int hh = ry * ry;
	int ww = rx * rx;
	int hhww = hh * ww;
	int x0 = rx;
	int dx = 0;

	// do the horizontal diameter
	for (int x = -rx; x <= rx; x++) GdiBrushPixel(dc, xc + x, yc, bhandle);

	// now do both halves at the same time, away from the diameter
	for (int y = 1; y <= ry; y++) {
		int x1 = x0 - (dx - 1);  // try slopes of dx - 1 or more
		for (; x1 > 0; x1--) if (x1 * x1 * hh + y * y * ww <= hhww) break;
		dx = x0 - x1;  // current approximation of the slope
		x0 = x1;

		for (int x = -x0; x <= x0; x++) {
			GdiBrushPixel(dc, xc + x, yc - y, bhandle);
			GdiBrushPixel(dc, xc + x, yc + y, bhandle);
		}
	}
}

void GdiDrawLine(Handle dc, int x1, int y1, int x2, int y2, int color) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	for (int x = x1; x < x2; x++) {
		int y = y1 + dy * (x - x1) / dx;
		GdiSetPixel(dc, x, y, color);
	}
}

void GdiBrushLine(Handle dc, int x1, int y1, int x2, int y2, Handle bhandle) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	for (int x = x1; x < x2; x++) {
		int y = y1 + dy * (x - x1) / dx;
		GdiBrushPixel(dc, x, y, bhandle);
	}
}

void GdiFlush(Handle dc) {
	DisplayObject* d = (DisplayObject*)ObSub(HandleRead(dc), DeviceObject, DisplayObject);
	memcpy(d->framebuffer, d->prebuffer, d->width * d->height * (d->bpp / 8));
}

int GdiRegisterHatchFunction(int hatch, void* function) {
	if (hatch > MAX_HATCH_FUNCTIONS) return -1;
	hatchFunctions[hatch] = (uintptr_t)function;
	return 0;
}

Handle GdiCreateBrush(BrushStyle style, Color color, Hatch hatch) {
	Brush* brush;
	BrushObCreate(style, color, hatch, &brush);
	return CreateHandle(brush);
}

Handle GdiCreateSolidBrush(Color color) {
	return GdiCreateBrush(SOLID, color, NONE);
}

Handle GdiCreateHatchedBrush(Color color, Hatch hatch) {
	return GdiCreateBrush(HATCHED, color, hatch);
}

uint32_t ScGetDC(InterruptRegisters* regs) {
	return GetDC(regs->ebx);
}

uint32_t ScGdiSetPixel(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiSetPixel(args[0], args[1], args[2], args[3]);
	return 0;
}

uint32_t ScGdiBrushPixel(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiBrushPixel(args[0], args[1], args[2], args[3]);
	return 0;
}

uint32_t ScGdiDrawRect(InterruptRegisters* regs) {
	GdiDrawRect(regs->ebx, (Rectangle*)regs->esi, regs->edi);
	return 0;
}

uint32_t ScGdiBrushRect(InterruptRegisters* regs) {
	GdiBrushRect(regs->ebx, (Rectangle*)regs->esi, regs->edi);
	return 0;
}

uint32_t ScGdiDrawRectOutline(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiDrawRectOutline(args[0], args[1], args[2], args[3], args[4], args[5]);
	return 0;
}

uint32_t ScGdiBrushRectOutline(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiBrushRectOutline(args[0], args[1], args[2], args[3], args[4], args[5]);
	return 0;
}

uint32_t ScGdiDrawLine(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiDrawLine(args[0], args[1], args[2], args[3], args[4], args[5]);
	return 0;
}

uint32_t ScGdiBrushLine(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiBrushLine(args[0], args[1], args[2], args[3], args[4], args[5]);
	return 0;
}

uint32_t ScGdiDrawEllipse(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiDrawEllipse(args[0], args[1], args[2], args[3], args[4], args[5]);
	return 0;
}

uint32_t ScGdiBrushEllipse(InterruptRegisters* regs) {
	int* args = (int*)regs->ebx;
	GdiBrushEllipse(args[0], args[1], args[2], args[3], args[4], args[5]);
	return 0;
}

uint32_t ScGdiCreateSolidBrush(InterruptRegisters* regs) {
	return GdiCreateSolidBrush(regs->ebx);
}

uint32_t ScGdiCreateHatchedBrush(InterruptRegisters* regs) {
	return GdiCreateHatchedBrush(regs->ebx, (Hatch)regs->esi);
}

uint32_t ScGdiFlush(InterruptRegisters* regs) {
	GdiFlush(regs->ebx);
	return 0;
}

static inline void GdiInitSyscalls() {
	GdiSyscallInstall(0, ScGetDC);
	GdiSyscallInstall(1, ScGdiSetPixel);
	GdiSyscallInstall(2, ScGdiBrushPixel);
	GdiSyscallInstall(3, ScGdiDrawRect);
	GdiSyscallInstall(4, ScGdiBrushRect);
	GdiSyscallInstall(5, ScGdiDrawRectOutline);
	GdiSyscallInstall(6, ScGdiBrushRectOutline);
	GdiSyscallInstall(7, ScGdiDrawLine);
	GdiSyscallInstall(8, ScGdiBrushLine);
	GdiSyscallInstall(9, ScGdiDrawEllipse);
	GdiSyscallInstall(10, ScGdiBrushEllipse);
	GdiSyscallInstall(11, ScGdiCreateSolidBrush);
	GdiSyscallInstall(12, ScGdiCreateHatchedBrush);
	GdiSyscallInstall(13, ScGdiFlush);
	Log("[GDI] Syscalls OK");
}

static inline void GdiInitHatches() {
	GdiRegisterHatchFunction(HORIZONTALLY, &HatchHorizontally);
	GdiRegisterHatchFunction(VERTICALLY, &HatchVertically);
	GdiRegisterHatchFunction(HORIZVERT, &HatchHorizVert);
	GdiRegisterHatchFunction(CHECKERBOARD, &HatchCheckerboard);
	Log("[GDI] Hatches OK");
}

int GdiKrnlInit() {
	Obj* dispobj = ObFind("Devices/Vga");
	Log("[GDI] Using device object '%s'", dispobj->name);
	DeviceObject* dispdev = (DeviceObject*)dispobj->data;
	Log("[GDI] Using display driver '%s': \"%s\"", dispdev->driver->name, dispdev->driver->description);
	mainDisplay = ObSub(dispdev, DeviceObject, DisplayObject);
	Log("[GDI] Using framebuffer @ %x (%dx%d)", mainDisplay->framebuffer, mainDisplay->width, mainDisplay->height);
	GdiInitHatches();
	GdiInitSyscalls();
	return 0;
}