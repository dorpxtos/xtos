#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h>

char islower(char c) {
	return c >= 'a' && c <= 'z';
}

char isupper(char c) {
	return c >= 'A' && c <= 'Z';
}

char isdigit(char c) {
	return c >= '0' && c <= '9';
}

char isalpha(char c) {
	return islower(c) || isupper(c);
}

char isalnum(char c) {
	return isalpha(c) || isdigit(c);
}

char toupper(char c) {
	if (islower(c)) return c - 'a' + 'A';
	else return c;
}

size_t strlen(char* str) {
	size_t length = 0;
	while (*str++) length++;
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

static const char* ITOA_NUMBERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/-_.,";

char* itoa(unsigned int value, char* buffer, int base) {
	if (value == 0) {
		*buffer = '0';
		*(buffer + 1) = 0;
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
