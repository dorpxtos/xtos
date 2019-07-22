#pragma once
#include <stddef.h>
#include <stdint.h>
#include <Sdt.h>

#define MADT_SIGNATURE "APIC"

#pragma pack(push)
struct Madt {
	Sdt sdt;
	uint32_t localApicAddress;
	uint32_t flags;
};
#pragma pack(pop, 1)

Madt* AcpiMadtLoad(uintptr_t a);