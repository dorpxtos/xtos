#include <stddef.h>
#include <stdint.h>
#include <Log.h>
#include <Gdt.h>

#define GDT_ENTRIES 8

const char* gdtEntryNames[] = {
	"NULL",
	"KCODE",
	"KDATA",
	"UCODE",
	"UDATA",
	"CODE16",
	"DATA16",
	"TSS"
};

extern "C" {
	GdtEntry gdt[GDT_ENTRIES];
	GdtPointer gdtr = {
		(uint16_t)(sizeof(gdt) - 1),
		(uint32_t)gdt
	};

	Tss tss = {
		0x0,
		0xC0EFFFF0,
		0x10
	};

	void LoadGdt();
}

static void LoadTss() {
	_asm {
		mov eax, 0x38
		ltr ax
	}
}

void GdtDump() {
	for (int i = 0; i < GDT_ENTRIES; i++) {
		Log("%s (%d) Base: %x_%x_%x Limit: %x Access: %b Gran: %b", gdtEntryNames[i], i, gdt[i].baseHigh, gdt[i].baseMiddle, gdt[i].baseLow, gdt[i].limitLow, gdt[i].access, gdt[i].granularity);
	}
}

void GdtInit() {
	// null pointer
	gdt[0].limitLow = 0;
	gdt[0].baseLow = 0;
	gdt[0].baseMiddle = 0;
	gdt[0].access = 0;
	gdt[0].granularity = 0;
	gdt[0].baseHigh = 0;

	// define kernel code
	gdt[1].limitLow = 0xffff;
	gdt[1].baseLow = 0x0000;
	gdt[1].baseMiddle = 0x00;
	gdt[1].access = 0b10011010;
	gdt[1].granularity = 0b11001111;
	gdt[1].baseHigh = 0x00;

	// define kernel data
	gdt[2].limitLow = 0xffff;
	gdt[2].baseLow = 0x0000;
	gdt[2].baseMiddle = 0x00;
	gdt[2].access = 0b10010010;
	gdt[2].granularity = 0b11001111;
	gdt[2].baseHigh = 0x00;

	// define user code
	gdt[3].limitLow = 0xffff;
	gdt[3].baseLow = 0x0000;
	gdt[3].baseMiddle = 0x00;
	gdt[3].access = 0b11111010;
	gdt[3].granularity = 0b11001111;
	gdt[3].baseHigh = 0x00;

	// define user data
	gdt[4].limitLow = 0xffff;
	gdt[4].baseLow = 0x0000;
	gdt[4].baseMiddle = 0x00;
	gdt[4].access = 0b11110010;
	gdt[4].granularity = 0b11001111;
	gdt[4].baseHigh = 0x00;

	// define 16-bit code
	gdt[5].limitLow = 0xffff;
	gdt[5].baseLow = 0x0000;
	gdt[5].baseMiddle = 0x00;
	gdt[5].access = 0b10011010;
	gdt[5].granularity = 0b10001111;
	gdt[5].baseHigh = 0x00;

	// define 16-bit data
	gdt[6].limitLow = 0xffff;
	gdt[6].baseLow = 0x0000;
	gdt[6].baseMiddle = 0x00;
	gdt[6].access = 0b10010010;
	gdt[6].granularity = 0b10001111;
	gdt[6].baseHigh = 0x00;

	// define TSS segment
	gdt[7].limitLow = (uint16_t)(sizeof(tss) & 0x0000ffff);
	gdt[7].baseLow = (uint16_t)((uint32_t)&tss & 0x0000ffff);
	gdt[7].baseMiddle = (uint8_t)(((uint32_t)&tss & 0x00ff0000) / 0x10000);
	gdt[7].access = 0b10001001;
	gdt[7].granularity = 0b00000000;
	gdt[7].baseHigh = (uint8_t)(((uint32_t)&tss & 0xff000000) / 0x1000000);

	Log("loading gdt");
	LoadGdt();
	Log("loaded gdt");
	GdtDump();
	Log("loading tss");
	LoadTss();
	Log("loaded tss");
}