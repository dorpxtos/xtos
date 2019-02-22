#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <PortIO.h>
#include <Idt.h>
#include <Log.h>
#include <File.h>
#include <MemoryAllocator.h>
#include <I8042.h>

#define PS2KBD_IRQ 1

bool echoKeypresses = true;
char layout[] = {
	// Lower
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
	'9', '0', '-', '=', '\b', /* Backspace */
	'\t',     /* Tab */
	'q', 'w', 'e', 'r', /* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
	0,      /* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
	'\'', '`',   0,    /* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
	'm', ',', '.', '/',   0,        /* Right shift */
	'*',
	0,  /* Alt */
	' ',  /* Space bar */
	0,  /* Caps lock */
	0,  /* 59 - F1 key ... > */
	1,   2,   3,   4,   5,   6,   7,   8,
	0,  /* < ... F10 */
	0,  /* 69 - Num lock*/
	0,  /* Scroll Lock */
	17, /* Home key */
	24, /* Up Arrow */
	30, /* Page Up */
	'-',
	27, /* Left Arrow */
	0,
	26, /* Right Arrow */
	'+',
	16, /* 79 - End key*/
	25, /* Down Arrow */
	31, /* Page Down */
	0,  /* Insert Key */
	0,  /* Delete Key */
	0,   0,   0,
	0,  /* F11 Key */
	0,  /* F12 Key */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* All other keys are undefined */

	// Upper
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
	'(', ')', '_', '+', '\b', /* Backspace */
	'\t',     /* Tab */
	'Q', 'W', 'E', 'R', /* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
	0,      /* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
	'"', '~',   0,    /* Left shift */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N',      /* 49 */
	'M', '<', '>', '?', 0,        /* Right shift */
	'*',
	0,  /* Alt */
	' ',  /* Space bar */
	0,  /* Caps lock */
	0,  /* 59 - F1 key ... > */
	1,   2,   3,   4,   5,   6,   7,   8,
	0,  /* < ... F10 */
	0,  /* 69 - Num lock*/
	0,  /* Scroll Lock */
	16,  /* Home key */
	24,  /* Up Arrow */
	30,  /* Page Up */
	'-',
	27,  /* Left Arrow */
	0,
	26,  /* Right Arrow */
	'+',
	17,  /* 79 - End key*/
	25,  /* Down Arrow */
	31,  /* Page Down */
	0,  /* Insert Key */
	0,  /* Delete Key */
	0,   0,   0,
	0,  /* F11 Key */
	0,  /* F12 Key */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* All other keys are undefined */

};

void I8042KeyboardIrq(InterruptRegisters* regs) {
	uint8_t scancode = inb(0x60);
	char chr;

	if (scancode & 0x80) {
		// Released
	} else {
		// Pressed
		chr = layout[scancode];

		if (echoKeypresses) {
			LogPrintChar(chr);
		}
	}
}

int I8042Init() {
	return 0;
	// Layout
	/*File* lf = FileOpen("COLEMAK.KBD");
	layout = (char*)MemoryAllocate(lf->size);
	FileReadAll(lf, (uint8_t*)layout);
	FileClose(lf);*/

	// IRQs
	//IrqInstall(PS2KBD_IRQ, &I8042KeyboardIrq);

	return 0;
}

int DllMain(short a, int b) {
	return I8042Init();
}
