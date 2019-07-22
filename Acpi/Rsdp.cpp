#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Log.h>
#include <Paging.h>
#include <ObjectManager.h>
#include <Rsdp.h>

#define BIOS_AREA_START KERNEL_MEMORY_BASE + 0xE0000
#define BIOS_AREA_END KERNEL_MEMORY_BASE + 0xFFFFF
#define RSDP_SIGNATURE "RSD PTR "

Obj* RsdpObCreate(Rsdp* t) {
	Obj* o = ObCreate("Acpi/Tables/Rsdp", "Rsdp", 0, NULL);
	o->data = (void*)t;
	return o;
}

bool AcpiRsdpChecksum(Rsdp* t) {
	uint8_t* bytes = (uint8_t*)t;
	char checksum = 0;
	for (int i = 0; i < sizeof(Rsdp); i++) {
		checksum += bytes[i];
	}
	if (checksum) {
		Log("Checksum for RSDP failed! Got %d instead of 0.", checksum);
		return false;
	}
	return true;
}

Rsdp* AcpiRsdpFind() {
	for (size_t i = BIOS_AREA_START; i < BIOS_AREA_END; i += 16) {
		if (!memcmp((void*)i, RSDP_SIGNATURE, 8)) {
			Rsdp* t = (Rsdp*)i;
			if (AcpiRsdpChecksum(t)) {
				RsdpObCreate(t);
				return t;
			}
		}
	}

	return NULL;
}