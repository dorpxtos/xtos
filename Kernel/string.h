#pragma once
#include <stddef.h>
#include <DeclSpec.h>

DllExport int islower(int);
DllExport int isupper(int);
DllExport int isalpha(int);
DllExport int isdigit(int);
DllExport int isalnum(int);
DllExport int isspace(int);
DllExport int isprint(int);
DllExport int isxdigit(int);
DllExport int toupper(int);
DllExport int tolower(int);
DllExport void* memcpy(void*, const void*, size_t);
DllExport void* memset(void*, int, size_t);
DllExport void* memset16(void*, int, size_t);
DllExport void* memset32(void*, int, size_t);
DllExport void* memset64(void*, int, size_t);
DllExport int memcmp(const void* s1, const void* s2, size_t n);
DllExport size_t strlen(const char*);
DllExport char* strupr(char*);
DllExport int strncmp(const char*, char*, size_t);
DllExport int strcmp(const char*, char*);
DllExport char* strchr(const char*, int);
DllExport char* strrchr(const char* cp, int ch);
DllExport char* strdup(const char*);
DllExport char* strcpy(char*, const char*);
DllExport char* strncpy(char*, const char*, size_t);
DllExport char* strcat(char* dest, const char* src);
DllExport size_t strcspn(const char*, const char*);
DllExport char* itoa(unsigned int, char*, int);
DllExport char* itoan(signed int, char*, int);