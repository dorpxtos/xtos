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

Obj* DsdtObCreate(Dsdt* t) {
	Obj* o = ObCreate("Acpi/Tables/Dsdt", "Dsdt", 0, NULL);
	o->data = (void*)t;
	return o;
}

Dsdt* AcpiDsdtLoad(uintptr_t a) {
	Dsdt* t = (Dsdt*)a;
	if (!t) return NULL;
	if (AcpiSdtChecksum(&t->sdt) && AcpiSdtValidateSignature(&t->sdt, DSDT_SIGNATURE)) {
		DsdtObCreate(t);
		//AmlParse(t->aml, t->sdt.length - sizeof(Sdt));
		return t;
	}
	return NULL;
}