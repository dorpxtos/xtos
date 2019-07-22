#pragma once
#include <stddef.h>
#include <Handle.h>

typedef void FILE;

DllExport int open(const char* filename, int omode);
DllExport int close(int fd);
DllExport int read(const int fd, const void* buffer, const unsigned buffer_size);
DllExport int write(const int fd, const void* buffer, const unsigned buffer_size);
DllExport int fputc(int c, FILE* stream);
DllExport int putchar(int);
DllExport int puts(char*);
DllExport FILE* fopen(const char*, const char*);
DllExport FILE* fclose(FILE*);
DllExport size_t fread(void*, size_t, size_t, FILE*);
DllExport size_t fwrite(const void* ptr, size_t, size_t, FILE*);
DllExport int fputs(const char*, FILE*);
DllExport long int ftell(FILE*);
DllExport long int fseek(FILE*, long int, int);
DllExport int ferror(FILE*);
DllExport void rewind(FILE*);
DllExport int putc(int, FILE*);
DllExport int fflush(FILE* stream);
DllExport char* fgets(char* str, int num, FILE* stream);
DllExport int fgetc(FILE* stream);