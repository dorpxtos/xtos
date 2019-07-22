#include <stdlib.h>
#include <Printf.h>
#include <Syscalls.h>
#include <LodePNG.h>
#include <Gdi.h>
#include <User.h>
#include <Desktop.h>

/* Brushes */
Handle desktopBackgroundBrushFront;
Handle desktopBackgroundBrushBack;
Handle windowBrush;

Handle hwnd;
Handle hwnd2;
Handle desktopDC;
Handle hwndDC;
Handle hwnd2DC;
Rectangle desktopBackgroundRect;

int x = 0;
int y = 0;

void DesktopInit() {
	desktopDC = GetDC(NULL);

	/* Brushes */
	desktopBackgroundBrushFront = GdiCreateHatchedBrush(0x0000FF, CHECKERBOARD);
	desktopBackgroundBrushBack = GdiCreateSolidBrush(0x000000);
	windowBrush = GdiCreateHatchedBrush(0xFF00FF, VERTICALLY);
	
	/* Background */
	desktopBackgroundRect.x = 0;
	desktopBackgroundRect.y = 0;
	desktopBackgroundRect.width = 800;
	desktopBackgroundRect.height = 600;

	/* Window */
	hwnd = CreateWindow("Desktop", 50, 25, 500, 500);
	ShowWindow(hwnd, SHOW);
	SetWindowAttribute(hwnd, WA_STYLE, GetWindowAttribute(hwnd, WA_STYLE) | WS_DECOR);
	hwndDC = GetDC(hwnd);

	hwnd2 = CreateWindow("Desktop2", 0, 0, 200, 200);
	ShowWindow(hwnd2, SHOW);
	SetWindowAttribute(hwnd2, WA_STYLE, GetWindowAttribute(hwnd2, WA_STYLE) | WS_DECOR);
	hwnd2DC = GetDC(hwnd2);

	Syscall(10, 0, 0, 0);
}

int ln = 0;

void DesktopDraw() {
	//GdiDrawRect(desktopDC, &desktopBackgroundRect, desktopBackgroundBrushBack);
	//GdiDrawRect(desktopDC, &desktopBackgroundRect, desktopBackgroundBrushFront);

	ln++;
	Rectangle r;
	r.x = 0;
	r.y = 0;
	r.width = 500;
	r.height = 500;
	GdiDrawRect(hwndDC, &r, desktopBackgroundBrushFront);
	GdiDrawRect(hwnd2DC, &r, windowBrush);

	GdiFlush(hwndDC);
	GdiFlush(hwnd2DC);
	GdiFlush(desktopDC);
	DrawWindow(hwnd);
	DrawWindow(hwnd2);
}