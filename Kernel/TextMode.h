#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern bool useTextMode;

void TextmodePrintChar(char);
void TextmodePrintString(char*);
void TextmodeClear();
extern "C" void TextmodeSetResolution(int, int);