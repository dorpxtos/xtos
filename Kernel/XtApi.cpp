#include <stdint.h>
#include <stddef.h>
#include <Log.h>
#include <XtApi.h>

uint32_t XtPrintChar(int c) {
	LogPrintChar(c);
	return 0;
}