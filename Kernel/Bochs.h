#pragma once
#include <PortIO.h>

#define BOCHS_DEBUG_PORT 0xE9

inline void BochsPrintChar(char c) {
	outb(BOCHS_DEBUG_PORT, c);
}

void BochsPrintString(char*);