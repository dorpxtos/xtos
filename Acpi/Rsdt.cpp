#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Log.h>
#include <Paging.h>
#include <ObjectManager.h>
#include <Sdt.h>
#include <Rsdp.h>
#include <Madt.h>
#include <Fadt.h>
#include <Rsdt.h>

Obj* RsdtObCreate(Rsdt* t) {
	Obj* o = ObCreate("Acpi/Tables/Rsdt", "Rsdt", 0, NULL);
	o->data = (void*)t;
	return o;
}

Rsdt* AcpiRsdtLoad(uintptr_t a) {
	Rsdt* t = (Rsdt*)a;
	if (!t) return NULL;
	if (AcpiSdtChecksum(&t->sdt) && AcpiSdtValidateSignature(&t->sdt, RSDT_SIGNATURE)) {
		RsdtObCreate(t);
		return t;
	}
	return NULL;
}

Rsdt* AcpiRsdtGet(Rsdp* p) {
	return AcpiRsdtLoad(KERNEL_MEMORY_BASE + p->rsdtAddress);
}

int AcpiRsdtRead(Rsdt* t) {
	int i;
	if (!t) return -1;
	for (i = 0; i < (t->sdt.length - sizeof(Sdt)) / sizeof(uintptr_t); i++) {
		uintptr_t va = KERNEL_MEMORY_BASE + t->tablePointers[i];
		char* sig = (char*)va;
		if (!memcmp(sig, MADT_SIGNATURE, SDT_SIGNATURE_LENGTH)) AcpiMadtLoad(va);
		else if (!memcmp(sig, FADT_SIGNATURE, SDT_SIGNATURE_LENGTH)) AcpiFadtLoad(va);
		else Log("Unknown table found while reading RSDT: \"%c%c%c%c\"", sig[0], sig[1], sig[2], sig[3]);
	}
	return i;
}