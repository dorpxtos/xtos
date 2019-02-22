#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <Idt.h>
#include <PortIO.h>
#include <Log.h>
#include <Task.h>
#include <Pit.h>

#define PIT_IRQ 0

uint64_t ticks = 0;

extern "C" void PitIrq(InterruptRegisters* registers) {
	// Tick
	ticks++;

	// Check if current process should yield
	if ((size_t)ticks % yieldInterval == 0 && schedulerEnabled) {
		TaskYield(registers);
	}
}

void PitSetFrequency(uint16_t freq) {
	outb(0x43, 0x36);
	outb(0x40, freq & 0xFF);
	outb(0x40, freq & 0x00FF);
}

void PitInit() {
	IrqInstall(PIT_IRQ, PitIrq);
	//PitSetFrequency(11931);

	LogPrint("PIT");
}