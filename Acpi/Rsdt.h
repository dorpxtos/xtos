#pragma once
#include <stdint.h>
#include <Rsdp.h>
#include <Sdt.h>

#define RSDT_SIGNATURE "RSDT"

#pragma pack(push)
struct Rsdt {
	Sdt sdt;
	uint32_t tablePointers[];
};
#pragma pack(pop, 1)

Rsdt* AcpiRsdtGet(Rsdp* p);
int AcpiRsdtRead(Rsdt* t);