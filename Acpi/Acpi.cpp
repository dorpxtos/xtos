#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <ObjectManager.h>
#include <PortIO.h>
#include <Fadt.h>
#include <Acpi.h>

Obj* AcpiObCreate(Acpi** t) {
	Obj* o = ObCreate("Acpi/Acpi", "Acpi", sizeof(Acpi), (void**)t);
	o->data = (void*)t;
	(*t)->rsdp = (Rsdp*)ObFind("Acpi/Tables/Rsdp")->data;
	(*t)->rsdt = (Rsdt*)ObFind("Acpi/Tables/Rsdt")->data;
	(*t)->madt = (Madt*)ObFind("Acpi/Tables/Madt")->data;
	(*t)->fadt = (Fadt*)ObFind("Acpi/Tables/Fadt")->data;
	(*t)->dsdt = (Dsdt*)ObFind("Acpi/Tables/Dsdt")->data;
	return o;
}

bool AcpiCheckIfEnabled(Acpi* t) {
	return (inb(t->fadt->pm1aControlBlock) & ACPI_PMCTRL_SCI_EN) == 1;
}

bool AcpiEnable(Acpi* t) {
	if (AcpiCheckIfEnabled(t)) {
		Log("ACPI already enabled");
		return true;
	} else {
		outb(t->fadt->smiCommandPort, t->fadt->acpiEnable);
		while (!AcpiCheckIfEnabled(t));
		Log("ACPI enabled!");
		return true;
	}
}

bool AcpiDisable(Acpi* t) {
	return true;
}