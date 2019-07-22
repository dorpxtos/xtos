#pragma once
#include <stdint.h>

#pragma pack(push)
struct Rsdp {
	char signature[8];
	uint8_t checksum;
	char oemId[6];
	uint8_t revision;
	uint32_t rsdtAddress;
};
#pragma pack(pop, 1)

Rsdp* AcpiRsdpFind();