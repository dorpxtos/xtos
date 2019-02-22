#pragma once
#include <stddef.h>
#include <stdint.h>
#include <Device.h>

#define MAX_ATA_DRIVES 64

struct AtaDrive {
	uint32_t id;
	char* name;
	uint8_t drive;
};

extern DeviceObject* ataPrimaryMasterDevice;

void AtaPioInit();
void AtaRead(uint8_t*, uint32_t, uint32_t, AtaDrive*);