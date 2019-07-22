#include <stdint.h>
#include <string.h>
#include <Paging.h>
#include <MemoryAllocator.h>
#include <TextMode.h>

#define CLEAR_COLOR 0x1000

static uint16_t* tmbuffer = (uint16_t*)(KERNEL_MEMORY_BASE + 0xb8000);
static int row = 0;
static int col = 0;

int rows = 25;
int cols = 80;
bool useTextMode = true;

void TextmodePrintChar(char c) {
	// TODO: Make less ugly
	if (c == '\f') {
		TextmodeClear();
		return;
	}

	if (c != '\n') {
		tmbuffer[row * cols + col] = (uint16_t)c | 0x1F00;
		col++;
	}

	if (col > cols || c == '\n') {
		col = 0;

		if (row < rows) {
			row++;
		}
		else {
			for (int i = 0; i < rows; i++) {
				for (int m = 0; m < cols; m++) {
					tmbuffer[i * cols + m] = tmbuffer[(i + 1) * cols + m];
				}
			}

			for (int i = 0; i < cols; i++) {
				tmbuffer[rows * cols + i] = 0;
			}
		}
	}
}

void TextmodePrintString(char* s) {
	while (*s) TextmodePrintChar(*s++);
}

void TextmodeClear() {
	memset((void*)tmbuffer, CLEAR_COLOR, rows * cols * 2);
	row = 0;
	col = 0;
}

extern "C" void TextmodeSetResolution(int h, int w) {
	rows = h;
	cols = w;
}