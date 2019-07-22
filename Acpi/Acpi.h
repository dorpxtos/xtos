#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <DeclSpec.h>
#include <ObjectManager.h>
#include <Rsdp.h>
#include <Rsdt.h>
#include <Fadt.h>
#include <Madt.h>
#include <Dsdt.h>

// Power management control block commands
#define ACPI_PMCTRL_SCI_EN		0x0001
#define ACPI_PMCTRL_BM_RLD		0x0002
#define ACPI_PMCTRL_GBL_RLS		0x0004
#define ACPI_PMCTRL_SLP_TYPX	0x1C00
#define ACPI_PMCTRL_SLP_EN		0x2000

#pragma pack(push)
struct Acpi {
	Rsdp* rsdp;
	Rsdt* rsdt;
	Fadt* fadt;
	Madt* madt;
	Dsdt* dsdt;
};
#pragma pack(pop, 1)

DllExport bool AcpiEnable(Acpi* t);
DllExport bool AcpiDisable(Acpi* t);
Obj* AcpiObCreate(Acpi** t);