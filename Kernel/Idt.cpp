#include <stdint.h>
#include <DeclSpec.h>
#include <Pic.h>
#include <PortIO.h>
#include <Log.h>
#include <Crash.h>
#include <Syscalls.h>
#include <Idt.h>

#define IDT_ENTRY_COUNT 256
#define IRQ_COUNT 16

#pragma pack(push,1)
struct IdtPointer {
	uint16_t limit;
	uint32_t base;
};

struct IdtEntry {
	uint16_t baseLow;
	uint16_t selector;
	uint8_t unused;
	uint8_t flags;
	uint16_t baseHigh;
};
#pragma pack(pop)

IdtPointer idtr;
IdtEntry idt[IDT_ENTRY_COUNT];

void* irqHandlers[IRQ_COUNT];

char* isrMessages[] = {
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

extern "C" {
	/* ISRs */
	extern void Isr0();
	extern void Isr1();
	extern void Isr2();
	extern void Isr3();
	extern void Isr4();
	extern void Isr5();
	extern void Isr6();
	extern void Isr7();
	extern void Isr8();
	extern void Isr9();
	extern void Isr10();
	extern void Isr11();
	extern void Isr12();
	extern void Isr13();
	extern void Isr14();
	extern void Isr15();
	extern void Isr16();
	extern void Isr17();
	extern void Isr18();
	extern void Isr19();
	extern void Isr20();
	extern void Isr21();
	extern void Isr22();
	extern void Isr23();
	extern void Isr23();
	extern void Isr24();
	extern void Isr25();
	extern void Isr26();
	extern void Isr27();
	extern void Isr28();
	extern void Isr29();
	extern void Isr30();
	extern void Isr31();

	/* IRQs */
	extern void Irq0();
	extern void Irq1();
	extern void Irq2();
	extern void Irq3();
	extern void Irq4();
	extern void Irq5();
	extern void Irq6();
	extern void Irq7();
	extern void Irq8();
	extern void Irq9();
	extern void Irq10();
	extern void Irq11();
	extern void Irq12();
	extern void Irq13();
	extern void Irq14();
	extern void Irq15();

	/* Syscalls */
	extern void SyscallStub();
}

static void IdtLoad() {
	_asm {
		lidt[idtr]
	}
}

static void RemapInterrupts() {
	// Remap the PIC
	PicRemap(0x20, 0x28);
}

static void SetGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
	idt[num].baseLow = base & 0xFFFF;
	idt[num].baseHigh = (base >> 16) & 0xFFFF;
	idt[num].selector = sel;
	idt[num].unused = 0;
	idt[num].flags = flags;
}

static void InitializeGates() {
	// ISRs
	SetGate(0, (uint32_t)Isr0, 0x08, 0b10001110);
	SetGate(1, (uint32_t)Isr1, 0x08, 0b10001110);
	SetGate(2, (uint32_t)Isr2, 0x08, 0b10001110);
	SetGate(3, (uint32_t)Isr3, 0x08, 0b10001110);
	SetGate(4, (uint32_t)Isr4, 0x08, 0b10001110);
	SetGate(5, (uint32_t)Isr5, 0x08, 0b10001110);
	SetGate(6, (uint32_t)Isr6, 0x08, 0b10001110);
	SetGate(7, (uint32_t)Isr7, 0x08, 0b10001110);
	SetGate(8, (uint32_t)Isr8, 0x08, 0b10001110);
	SetGate(9, (uint32_t)Isr9, 0x08, 0b10001110);
	SetGate(10, (uint32_t)Isr10, 0x08, 0b10001110);
	SetGate(11, (uint32_t)Isr11, 0x08, 0b10001110);
	SetGate(12, (uint32_t)Isr12, 0x08, 0b10001110);
	SetGate(13, (uint32_t)Isr13, 0x08, 0b10001110);
	SetGate(14, (uint32_t)Isr14, 0x08, 0b10001110);
	SetGate(15, (uint32_t)Isr15, 0x08, 0b10001110);
	SetGate(16, (uint32_t)Isr16, 0x08, 0b10001110);
	SetGate(17, (uint32_t)Isr17, 0x08, 0b10001110);
	SetGate(18, (uint32_t)Isr18, 0x08, 0b10001110);
	SetGate(19, (uint32_t)Isr19, 0x08, 0b10001110);
	SetGate(20, (uint32_t)Isr20, 0x08, 0b10001110);
	SetGate(21, (uint32_t)Isr21, 0x08, 0b10001110);
	SetGate(22, (uint32_t)Isr22, 0x08, 0b10001110);
	SetGate(23, (uint32_t)Isr23, 0x08, 0b10001110);
	SetGate(24, (uint32_t)Isr24, 0x08, 0b10001110);
	SetGate(25, (uint32_t)Isr25, 0x08, 0b10001110);
	SetGate(26, (uint32_t)Isr26, 0x08, 0b10001110);
	SetGate(27, (uint32_t)Isr27, 0x08, 0b10001110);
	SetGate(28, (uint32_t)Isr28, 0x08, 0b10001110);
	SetGate(29, (uint32_t)Isr29, 0x08, 0b10001110);
	SetGate(30, (uint32_t)Isr30, 0x08, 0b10001110);
	SetGate(31, (uint32_t)Isr31, 0x08, 0b10001110);

	// Remapping
	RemapInterrupts();

	// IRQs
	SetGate(32, (uint32_t)Irq0, 0x08, 0b10001110);
	SetGate(33, (uint32_t)Irq1, 0x08, 0b10001110);
	SetGate(34, (uint32_t)Irq2, 0x08, 0b10001110);
	SetGate(35, (uint32_t)Irq3, 0x08, 0b10001110);
	SetGate(36, (uint32_t)Irq4, 0x08, 0b10001110);
	SetGate(37, (uint32_t)Irq5, 0x08, 0b10001110);
	SetGate(38, (uint32_t)Irq6, 0x08, 0b10001110);
	SetGate(39, (uint32_t)Irq7, 0x08, 0b10001110);
	SetGate(40, (uint32_t)Irq8, 0x08, 0b10001110);
	SetGate(41, (uint32_t)Irq9, 0x08, 0b10001110);
	SetGate(42, (uint32_t)Irq10, 0x08, 0b10001110);
	SetGate(43, (uint32_t)Irq11, 0x08, 0b10001110);
	SetGate(44, (uint32_t)Irq12, 0x08, 0b10001110);
	SetGate(45, (uint32_t)Irq13, 0x08, 0b10001110);
	SetGate(46, (uint32_t)Irq14, 0x08, 0b10001110);
	SetGate(47, (uint32_t)Irq15, 0x08, 0b10001110);

	// Syscalls
	SetGate(128, (uint32_t)SyscallStub, 0x08, 0b11101110);
}

int IrqInstall(uint8_t num, void(*handler)(InterruptRegisters*)) {
	// Print message
	LogPrint("Installing IRQ# %d", num);

	// Check if a handler is already registered
	if (irqHandlers[num]) return -1;

	// Check if IRQ# is too high
	if (num > IRQ_COUNT) return -2;

	// Register handler
	irqHandlers[num] = handler;

	// Success!
	return 0;
}

int IrqUninstall(uint8_t num) {
	// Clear handler
	irqHandlers[num] = 0;

	// Success!
	return 0;
}

extern "C" {
	void IsrHandler(InterruptRegisters* registers) {
		CrashCritical(isrMessages[registers->int_no], registers);
		while (1) {}
	}

	void IrqHandler(InterruptRegisters* registers) {
		// Check if handler exists
		if (irqHandlers[registers->int_no - 32]) {
			// Call the handler
			((void(*)(InterruptRegisters*)) irqHandlers[registers->int_no - 32])(registers);
		}

		// Send EOI
		PicSendEoi((uint8_t)registers->int_no);
	}
}

void IdtInit() {
	// Set base and limit
	idtr.base = (uint32_t)&idt;
	idtr.limit = IDT_ENTRY_COUNT * sizeof(IdtEntry) - 1;

	// Load the IDT
	IdtLoad();
	
	// Initialize Gates
	InitializeGates();

	// Enable interrupts
	EnableInterrupts();
	
	// Success!
	LogPrint("IRQ");
}