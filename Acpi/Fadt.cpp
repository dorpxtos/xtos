#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Log.h>
#include <Paging.h>
#include <ObjectManager.h>
#include <Aml.h>
#include <Sdt.h>
#include <Dsdt.h>
#include <Fadt.h>

Obj* FadtObCreate(Fadt* t) {
	Obj* o = ObCreate("Acpi/Tables/Fadt", "Fadt", 0, NULL);
	o->data = (void*)t;
	return o;
}

Fadt* AcpiFadtLoad(uintptr_t a) {
	Fadt* t = (Fadt*)a;
	if (!t) return NULL;
	if (AcpiSdtChecksum(&t->sdt) && AcpiSdtValidateSignature(&t->sdt, FADT_SIGNATURE)) {
		FadtObCreate(t);
		AcpiDsdtLoad(KERNEL_MEMORY_BASE + t->dsdt);
		return t;
	}
	return NULL;
}