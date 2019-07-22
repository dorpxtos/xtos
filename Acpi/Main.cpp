#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <DeclSpec.h>
#include <Log.h>
#include <Rsdp.h>
#include <Rsdt.h>
#include <Acpi.h>

Rsdp* rsdp;
Rsdt* rsdt;

DllExport int AcpiInit() {
	Log("initializing ACPI");
	rsdp = AcpiRsdpFind();
	if (!rsdp) {
		Log("ERROR: RSDP is null");
		return -1;
	}
	Log("RSDP OK");
	rsdt = AcpiRsdtGet(rsdp);
	if (!rsdt) {
		Log("ERROR: RSDT is null");
		return -1;
	}
	AcpiRsdtRead(rsdt);
	Log("RSDT OK");
	Acpi* acpi;
	AcpiObCreate(&acpi);
	AcpiEnable(acpi);
	Log("initialized ACPI");
	return 0;
}