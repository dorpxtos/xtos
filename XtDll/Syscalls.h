#pragma once
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <Handle.h>

DllExport void XtPrintChar(int);
DllExport Handle XtGetRunningThreadHandle();
DllExport Handle XtFileOpen(char*);
DllExport int XtFileRead(Handle, uint8_t*, int);
DllExport int XtFileWrite(Handle, uint8_t*, int);
DllExport void XtPrintString(char*);