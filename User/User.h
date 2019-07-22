#pragma once
#include <stdbool.h>
#include <DeclSpec.h>
#include <Handle.h>

#define WA_STYLE 1
#define WA_PRIVATE1 30
#define WA_PRIVATE2 31
#define WS_DECOR 1

typedef enum ShowMode {
	HIDE,
	SHOW
} ShowMode;

DllExport Handle CreateWindow(char* name, int x, int y, int w, int h);
DllExport bool ShowWindow(Handle hwnd, ShowMode mode);
DllExport Handle WindowGetDisplay(Handle hwnd);
DllExport void DrawWindow(Handle hwnd);
DllExport uint32_t SetWindowAttribute(Handle whdl, int attr, uint32_t value);
DllExport uint32_t GetWindowAttribute(Handle whdl, int attr);