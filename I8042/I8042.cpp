#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <DeclSpec.h>
#include <PortIO.h>
#include <Log.h>
#include <Idt.h>
#include <File.h>
#include <MemoryAllocator.h>
#include <ObjectManager.h>
#include <Io.h>
#include <IoResource.h>
#include <Driver.h>
#include <Device.h>
#include <I8042.h>

#define PS2KBD_IRQ 1

struct I8042Keyboard {
	int scancodePressed;
	int charPressed;
	int totalKeypresses;
	int totalKeyreleases;
	bool echoKeypresses;
};

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

DriverObject* i8042Driver;
DeviceObject* i8042Device;
DriverObject* i8042KeyboardDriver;
DeviceObject* i8042KeyboardDevice;
IoResourceList* i8042iorl;
IoResourceList* i8042kbdiorl;

IoStatus I8042DevRead(IoStack* req) {
	return IOSTATUS_SUCCESS;
}

IoStatus I8042DevWrite(IoStack* req) {
	return IOSTATUS_SUCCESS;
}

IoStatus I8042KeyboardDevRead(IoStack* req) {
	I8042Keyboard* kbd = (I8042Keyboard*)req->device->extendedData;
	for (int i = 0; i < req->parameters.read.count; i++) {
		int lastPresses = kbd->totalKeypresses;
		while (lastPresses == kbd->totalKeypresses);
		req->parameters.read.buffer[i] = kbd->charPressed;
	}
	return IOSTATUS_SUCCESS;
}

void I8042KeyboardIrq(InterruptRegisters* regs) {
	I8042Keyboard* kbd = (I8042Keyboard*)i8042KeyboardDevice->extendedData;
	kbd->scancodePressed = inb(0x60);

	if (kbd->scancodePressed & 0x80) {
		// Released
		kbd->charPressed = 0;
		kbd->scancodePressed = 0;
		kbd->totalKeyreleases++;
	}
	else {
		// Pressed
		kbd->charPressed = layout[kbd->scancodePressed];
		kbd->totalKeypresses++;

		if (kbd->echoKeypresses) {
			LogPrintChar(kbd->charPressed);
		}
	}
}

void I8042DevInit() {
	/* I8042 */
	Log("initializing i8042");
	DriverObCreate("I8042", "Intel 8042", &i8042Driver);
	IoCreateDevice("I8042", DEVICE_TYPE_SYSTEM, sizeof(I8042Keyboard), i8042Driver, &i8042Device);
	DriverObRegisterFunction(i8042Driver, DRIVER_FUNCTION_READ, &I8042DevRead);
	DriverObRegisterFunction(i8042Driver, DRIVER_FUNCTION_WRITE, &I8042DevWrite);
	Log("configuring resources for i8042");
	IoResourceListCreate(2, &i8042iorl);
	IoResource* i8042resio = IoResourceGetNextDescriptor(i8042iorl);
	i8042resio->type = PORTRANGE;
	i8042resio->value.port.minimum = 0x60;
	i8042resio->value.port.maximum = 0x61;
	i8042resio->value.port.length = 2;
	IoResourceListLinkWithDevice(i8042Device, i8042iorl);
	Log("initialized i8042");

	/* I8042 Keyboard */
	Log("initializing i8042 keyboard");
	DriverObCreate("I8042Keyboard", "Intel 8042 Keyboard", &i8042KeyboardDriver);
	IoCreateDevice("I8042Keyboard", DEVICE_TYPE_KEYBOARD, 0, i8042KeyboardDriver, &i8042KeyboardDevice);
	DriverObRegisterFunction(i8042KeyboardDriver, DRIVER_FUNCTION_READ, &I8042KeyboardDevRead);
	Log("configuring resources for i8042 keyboard");
	IoResourceListCreate(2, &i8042kbdiorl);
	IoResource* i8042kbdresio = IoResourceGetNextDescriptor(i8042kbdiorl);
	i8042kbdresio->type = INTERRUPT;
	i8042kbdresio->value.interrupt.irq = PS2KBD_IRQ;
	IoResourceListLinkWithDevice(i8042KeyboardDevice, i8042kbdiorl);
	IrqInstall(PS2KBD_IRQ, &I8042KeyboardIrq);
	Log("initialized i8042 keyboard");
}

DllExport int I8042Init() {
	I8042DevInit();
	return 0;
}

int DllMain(short a, int b) {
	return I8042Init();
}
