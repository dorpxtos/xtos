#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Log.h>
#include <Sdt.h>

bool AcpiSdtChecksum(Sdt* t) {
	uint8_t* bytes = (uint8_t*)t;
	char checksum = 0;
	for (int i = 0; i < t->length; i++) {
		checksum += bytes[i];
	}
	if (checksum) {
		Log("Checksum for %s failed! Got %d instead of 0.", t->signature, checksum);
		return false;
	}
	return true;
}

bool AcpiSdtValidateSignature(Sdt* t, char* sig) {
	return !memcmp(t->signature, sig, SDT_SIGNATURE_LENGTH);
}