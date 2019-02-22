#pragma once
#include <stddef.h>
#include <DeclSpec.h>

DllExport char islower(char);
DllExport char isupper(char);
DllExport char isalpha(char);
DllExport char isdigit(char);
DllExport char isalnum(char);
DllExport char toupper(char);
DllExport void* memcpy(void*, const void*, size_t);
DllExport void* memset(void*, int, size_t);
DllExport void* memset16(void*, int, size_t);
DllExport void* memset32(void*, int, size_t);
DllExport void* memset64(void*, int, size_t);
DllExport char* itoa(unsigned int, char*, int);
DllExport char* itoan(signed int, char*, int);
DllExport size_t strlen(char*);
DllExport char* strupr(char*);
DllExport int strncmp(const char*, char*, size_t);
DllExport int strcmp(const char* s1, char* s2);