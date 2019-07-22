#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <Syscalls.h>

int open(const char* filename, int omode) {
	return XtFileOpen(filename);
}

int close(int fd) {
	return XtFileClose(fd);
}

int read(const int fd, const void* buffer, const unsigned buffer_size) {
	return XtFileRead(fd, buffer, buffer_size);
}

int write(const int fd, const void* buffer, const unsigned buffer_size) {
	return XtFileWrite(fd, buffer, buffer_size);
}

FILE* fopen(const char* filename, const char* mode) {
	return (FILE*)XtFileOpen(filename);
}

FILE* fclose(FILE* stream) {
	return 0;
}

size_t fread(void* buffer, size_t size, size_t count, FILE* stream) {
	size_t read = XtFileRead(stream, buffer, size * count);
	return read / size;
}

size_t fwrite(const void* ptr, size_t size, size_t count, FILE* stream) {
	size_t wrote = XtFileWrite(stream, ptr, size * count);
	return wrote / size;
}

int fputs(const char* str, FILE* stream) {
	fwrite(str, strlen(str), 1, stream);
	return 0;
}

long int ftell(FILE* stream) {
	return XtFileTell(stream);
}

long int fseek(FILE* stream, long int offset, int origin) {
	return XtFileSeek(stream, offset, origin);
}

int ferror(FILE* stream) {
	return 0;
}

int fflush(FILE* stream) {
	return 0;
}

char* fgets(char* str, int num, FILE* stream) {
	fread(str, num, 1, stream);
	return str;
}

void rewind(FILE* stream) {

}

int fputc(int c, FILE* stream) {
	if ((uint32_t)stream < 10) {
		XtPrintChar(c);
		return c;
	}
	fwrite(&c, 1, 1, stream);
	return c;
}

int fgetc(FILE* stream) {
	int c;
	fread(&c, 1, 1, stream);
	return c;
}