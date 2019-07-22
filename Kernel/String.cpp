#include <stddef.h>
#include <stdint.h>
#include <MemoryAllocator.h>
#include <Log.h>
#include <string.h>

int islower(int c) {
	return c >= 'a' && c <= 'z';
}

int isupper(int c) {
	return c >= 'A' && c <= 'Z';
}

int isdigit(int c) {
	return c >= '0' && c <= '9';
}

int isalpha(int c) {
	return islower(c) || isupper(c);
}

int isalnum(int c) {
	return isalpha(c) || isdigit(c);
}

int isspace(int c) {
	return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

int isprint(int c) {
	return (c > 0x1f && c != '\d');
}

int isxdigit(int c) {
	return (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'));
}

int toupper(int c) {
	if (islower(c)) return c - 'a' + 'A';
	else return c;
}

int tolower(int c) {
	if (isupper(c)) return c - 'A' + 'a';
	else return c;
}

size_t strlen(const char* str) {
	char* s = (char*)str;
	size_t length = 0;
	while (*s++) length++;
	return length;
}

void* memcpy(void* destination, const void* source, size_t count) {
	char* destinationBytes = (char*)destination;
	char* sourceBytes = (char*)source;

	for (size_t i = 0; i < count; i++) {
		destinationBytes[i] = sourceBytes[i];
	}

	return destination;
}

void* memset(void* destination, int value, size_t count) {
	uint8_t* destinationBytes = (uint8_t*)destination;
	for (size_t i = 0; i < count; i++) {
		destinationBytes[i] = value;
	}
	return destination;
}

void* memset16(void* destination, int value, size_t count) {
	uint16_t* destinationWords = (uint16_t*)destination;
	for (size_t i = 0; i < count; i++) {
		destinationWords[i] = value;
	}
	return destination;
}

void* memset32(void* destination, int value, size_t count) {
	uint32_t* destinationDwords = (uint32_t*)destination;
	for (size_t i = 0; i < count; i++) {
		destinationDwords[i] = value;
	}
	return destination;
}

void* memset64(void* destination, int value, size_t count) {
	uint64_t* destinationQwords = (uint64_t*)destination;
	for (size_t i = 0; i < count; i++) {
		destinationQwords[i] = value;
	}
	return destination;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	uint8_t* a1 = (uint8_t*)s1;
	uint8_t* a2 = (uint8_t*)s2;
	uint8_t u1, u2;

	for (; n--; a1++, a2++) {
		u1 = *(uint8_t*)a1;
		u2 = *(uint8_t*)a2;
		if (u1 != u2) {
			return (u1 - u2);
		}
	}
	
	return 0;
}

void strrev(char* str) {
	size_t length = strlen(str);
	size_t i;
	size_t j;

	for (i = 0, j = length - 1; i < j; i++, j--) {
		char a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}

char* strupr(char* str) {
	char* s = str;
	while (*s) *s++ = toupper(*s);
	return str;
}

int strncmp(const char* s1, char* s2, size_t n) {
	if (n == 0) return 0;
	do {
		if (*s1 != *s2++) return (*(uint8_t*)s1 - *(uint8_t*)--s2);
		if (*s1++ == 0) break;
	} while (--n != 0);
	return 0;
}

int strcmp(const char* s1, char* s2) {
	while (1) {
		if (*s1 != *s2++) return (*(uint8_t*)s1 - *(uint8_t*)--s2);
		if (*s1++ == 0) break;
	}
	return 0;
}

char* strchr(const char* p, int ch) {
	char c = ch;

	for (;; ++p) {
		if (*p == c) return (char*)p;
		if (!*p) return NULL;
	}
}

char* strrchr(const char* cp, int ch) {
	char* save;
	char c;

	for (save = (char*)0; (c = *cp); cp++) {
		if (c == ch)
			save = (char*)cp;
	}

	return save;
}

char* strdup(const char* s) {
	size_t len = strlen(s) + 1;
	void* n = MemoryAllocate(len);
	if (!n) return NULL;
	return (char*)memcpy(n, s, len);
}

char* strcpy(char* dst, const char* src) {
	memcpy(dst, src, strlen(src));
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	memcpy(dst, src, n);
	return dst;
}

char* strcat(char* dest, const char* src) {
	strcpy(dest + strlen(dest), src);
	return dest;
}

size_t strcspn(const char* s1, const char* s2) {
	const char* p, *spanp;
	char c, sc;

	/*
	 * Stop as soon as we find any character from s2.  Note that there
	 * must be a NUL in s2; it suffices to stop when we find that, too.
	 */
	for (p = s1;;) {
		c = *p++;
		spanp = s2;
		do {
			if ((sc = *spanp++) == c)
				return (p - 1 - s1);
		} while (sc != 0);
	}

	return 0;
}

static const char* ITOA_NUMBERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/-_.,";

char* itoa(unsigned int value, char* buffer, int base) {
	if (value == 0) {
		*buffer = '0';
		*(buffer+1) = 0;
		return buffer;
	}

	char* str = buffer;
	
	while (value != 0) {
		*str++ = ITOA_NUMBERS[value % base];
		value /= base;
	}
	
	strrev(buffer);

	buffer[strlen(buffer)] = 0;

	return buffer;
}

char* itoan(signed int value, char* buffer, int base) {
	if (value == 0) {
		*buffer = '0';
		*(buffer + 1) = 0;
		return buffer;
	}

	char* str = buffer;
	bool isNegative = false;

	if (value < 0) {
		isNegative = true;
		value = -value;
	}

	while (value != 0) {
		*str++ = ITOA_NUMBERS[value % base];
		value /= base;
	}

	if (isNegative) {
		*str++ = '-';
	}

	strrev(buffer);

	buffer[strlen(buffer)] = 0;

	return buffer;
}