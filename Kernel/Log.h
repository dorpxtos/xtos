#pragma once
#include <DeclSpec.h>
#include <Utf8.h>

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION __FUNCTION__ ":" S2(__LINE__)
#define Log(MSG, ...) LogPrint((char*)("(" LOCATION ") " MSG), __VA_ARGS__)
#define Breakpoint Log("Breakpoint"); _asm {int 3}

DllExport void LogPrint(char* format, ...);
DllExport void LogPrintChar(utf8_int32_t);
void LogDevInit();