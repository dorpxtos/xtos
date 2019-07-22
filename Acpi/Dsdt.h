#pragma once
#include <stddef.h>
#include <stdint.h>
#include <Sdt.h>

#define DSDT_SIGNATURE "DSDT"

#pragma pack(push)
struct Dsdt {
	Sdt sdt;
	uint8_t aml[];
};
#pragma pack(pop, 1)

Dsdt* AcpiDsdtLoad(uintptr_t a);