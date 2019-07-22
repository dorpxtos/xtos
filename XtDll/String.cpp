#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <math.h>
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
	return c == ' ' || c == '\t' || c == '\n';
}

int toupper(int c) {
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

void* memmove(void* dest, const void* src, size_t len) {
	char* d = dest;
	const char* s = src;
	if (d < s)
		while (len--)
			*d++ = *s++;
	else {
		char* lasts = s + (len - 1);
		char* lastd = d + (len - 1);
		while (len--)
			*lastd-- = *lasts--;
	}
	return dest;
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

char* strchr(const char* p, int ch) {
	char c = ch;

	for (;; ++p) {
		if (*p == c) return (char*)p;
		if (!*p) return NULL;
	}
}

char* strcpy(char* dst, const char* src) {
	memcpy(dst, src, strlen(src));
	return dst;
}

char* strncpy(char* dst, const char* src, size_t num) {
	memcpy(dst, src, num);
	return dst;
}

char* strstr(const char* s1, const char* s2) {
	const char* p = s1;
	const size_t len = strlen(s2);

	for (; (p = strchr(p, *s2)) != 0; p++) {
		if (strncmp(p, s2, len) == 0) return (char*)p;
	}
	return 0;
}

double strtod(const char* str, char** endptr) {
	double number;
	int exponent;
	int negative;
	char* p = (char* )str;
	double p10;
	int n;
	int num_digits;
	int num_decimals;

	// Skip leading whitespace
	while (isspace(*p)) p++;

	// Handle optional sign
	negative = 0;
	switch (*p) {
	case '-': negative = 1; // Fall through to increment position
	case '+': p++;
	}

	number = 0.;
	exponent = 0;
	num_digits = 0;
	num_decimals = 0;

	// Process string of digits
	while (isdigit(*p)) {
		number = number * 10. + (*p - '0');
		p++;
		num_digits++;
	}

	// Process decimal part
	if (*p == '.') {
		p++;

		while (isdigit(*p)) {
			number = number * 10. + (*p - '0');
			p++;
			num_digits++;
			num_decimals++;
		}

		exponent -= num_decimals;
	}

	if (num_digits == 0) {
		//errno = ERANGE;
		return 0.0;
	}
	
	// Correct for sign
	if (negative) number = -number;

	// Process an exponent string
	if (*p == 'e' || *p == 'E') {
		// Handle optional sign
		negative = 0;
		switch (*++p) {
		case '-': negative = 1;   // Fall through to increment pos
		case '+': p++;
		}

		// Process string of digits
		n = 0;
		while (isdigit(*p)) {
			n = n * 10 + (*p - '0');
			p++;
		}

		if (negative) {
			exponent -= n;
		}
		else {
			exponent += n;
		}
	}

	if (exponent < DBL_MIN_EXP || exponent > DBL_MAX_EXP) {
		//errno = ERANGE;
		return HUGE_VAL;
	}

	// Scale the result
	p10 = 10.;
	n = exponent;
	if (n < 0) n = -n;
	while (n) {
		if (n & 1) {
			if (exponent < 0) {
				number /= p10;
			}
			else {
				number *= p10;
			}
		}
		n >>= 1;
		p10 *= p10;
	}

	//if (number == HUGE_VAL) errno = ERANGE;
	if (endptr) *endptr = p;

	return number;
}

float strtof(const char* str, char** endptr) {
	return (float)strtod(str, endptr);
}


long double strtold(const char* str, char** endptr) {
	return strtod(str, endptr);
}

double atof(const char* str) {
	return strtod(str, NULL);
}