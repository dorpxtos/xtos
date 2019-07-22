#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <DeclSpec.h>
#include <Handle.h>
#include <Video.h>

#define WA_STYLE 1
#define WA_PRIVATE1 30
#define WA_PRIVATE2 31
#define WS_DECOR 1

typedef enum ShowMode {
	HIDE,
	SHOW
} ShowMode;

typedef struct Window {
	char* title;
	int x;
	int y;
	int height;
	int width;
	uint32_t attributes[32];
	ShowMode showMode;
	DisplayObject* windisp;
	DisplayObject* display;
	DeviceObject* displayDevice;
} Window;

DllExport void UserKrnlInit();
DllExport Handle UserCreateWindow(char*, int, int, int, int);
DllExport bool UserShowWindow(Handle, ShowMode);