#pragma once
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <Handle.h>
#include <Graphics.h>

DllExport void XtPrintChar(int c);
DllExport Handle XtGetRunningThreadHandle();
DllExport Handle XtFileOpen(char* name);
DllExport Handle XtFileClose(Handle f);
DllExport int XtFileRead(Handle f, uint8_t* buffer, int count);
DllExport int XtFileWrite(Handle f, uint8_t* buffer, int count);
DllExport int XtFileTell(Handle f);
DllExport unsigned long int XtFileSeek(Handle f, unsigned long int offset, int whence);
DllExport void XtPrintString(char* msg);
DllExport void* XtAllocPages(size_t pages);
