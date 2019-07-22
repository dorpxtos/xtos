#pragma once
#include <stdint.h>

#define SDT_SIGNATURE_LENGTH 4

#pragma pack(push)
struct Sdt {
	char signature[SDT_SIGNATURE_LENGTH];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemId[6];
	char oemTableId[8];
	uint32_t oemRevision;
	uint32_t creatorId;
	uint32_t creatorRevision;
};
#pragma pack(pop, 1)

bool AcpiSdtChecksum(Sdt* t);
bool AcpiSdtValidateSignature(Sdt* t, char* sig);