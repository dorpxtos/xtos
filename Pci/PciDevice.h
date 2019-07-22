#pragma once
#include <stdint.h>

#pragma push(pack)
struct PciHeader {
	uint16_t deviceId;
	uint16_t vendorId;
	uint16_t status;
	uint16_t command;
	uint8_t classCode;
	uint8_t subclass;
	uint8_t progIf;
	uint8_t revisionId;
	uint8_t bist;
	uint8_t headerType;
	uint8_t latencyTimer;
	uint8_t cacheLineSize;
};
#pragma pop(pack,1)