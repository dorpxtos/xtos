#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Log.h>
#include <Paging.h>
#include <ObjectManager.h>
#include <Sdt.h>
#include <Madt.h>

#define RT_PLA 0
#define RT_IOAPIC 1
#define RT_ISO 2
#define RT_NMI 4
#define RT_LAAO 5

#pragma pack(push)
struct ProcessorLocalApic {
	uint8_t acpiProcessorId;
	uint8_t apicId;
	uint32_t flags;
};

struct IoApic {
	uint8_t ioApicId;
	uint8_t reserved;
	uint32_t apicAddress;
	uint32_t globalSystemInterruptBase;
};

struct InterruptSourceOverride {
	uint8_t busSource;
	uint8_t irqSource;
	uint32_t globalSystemInterrupt;
	uint16_t flags;
};

struct NonMaskableInterrupts {
	uint8_t acpiProcessorId;
	uint16_t flags;
	uint8_t lint;
};

struct LocalApicAddressOverride {
	uint16_t reserved;
	uint64_t address;
};
#pragma pack(pop, 1)

char* recordTypeStrings[] = {
	"Processor Local APIC",
	"I/O APIC",
	"Interrupt Source Override",
	"?",
	"Non-Maskable Interrupts",
	"Local APIC Address Override"
};

Obj* MadtObCreate(Madt* t) {
	Obj* o = ObCreate("Acpi/Tables/Madt", "Madt", 0, NULL);
	o->data = (void*)t;
	return o;
}

void AcpiMadtParse(Madt* t) {
	uint8_t* bytes = (uint8_t*)t;
	int recordNum = 0;
	for (int i = sizeof(Madt); i < t->sdt.length; recordNum++) {
		uint8_t recordType = bytes[i++];
		uint8_t recordLength = bytes[i++];
		//Log("MADT Record %d (%d): %s", recordNum, recordLength, recordTypeStrings[recordType]);
		if (recordType == RT_PLA) {
			ProcessorLocalApic* e = (ProcessorLocalApic*)(bytes + i);
			Log("[PLA] Processor: %x APIC: %x", e->acpiProcessorId, e->apicId);
		}
		else if (recordType == RT_IOAPIC) {
			IoApic* e = (IoApic*)(bytes + i);
			Log("[IOAPIC] APIC: %x Address: %x Base: %x", e->ioApicId, e->apicAddress, e->globalSystemInterruptBase);
		}
		else if (recordType == RT_ISO) {
			InterruptSourceOverride* e = (InterruptSourceOverride*)(bytes + i);
			Log("[ISO] Bus: %x IRQ: %x GSI: %x", e->busSource, e->irqSource, e->globalSystemInterrupt);
		}
		else if (recordType == RT_NMI) {
			NonMaskableInterrupts* e = (NonMaskableInterrupts*)(bytes + i);
			Log("[NMI] Processor: %x LINT: %x", e->acpiProcessorId, e->lint);
		}
		else if (recordType == RT_LAAO) {
			LocalApicAddressOverride* e = (LocalApicAddressOverride*)(bytes + i);
			Log("[LAAO] Address: %x", e->address);
		}
		else {
			Log("Unknown record type");
		}
		i += recordLength - 2;
	}
}

Madt* AcpiMadtLoad(uintptr_t a) {
	Madt* t = (Madt*)a;
	if (!t) return NULL;
	if (AcpiSdtChecksum(&t->sdt) && AcpiSdtValidateSignature(&t->sdt, MADT_SIGNATURE)) {
		MadtObCreate(t);
		AcpiMadtParse(t);
		return t;
	}
	return NULL;
}