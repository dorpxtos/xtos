#include <Bochs.h>

void BochsPrintString(char* s) {
	while (*s) BochsPrintChar(*s++);
}