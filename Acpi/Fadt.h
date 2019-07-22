#pragma once
#include <stddef.h>
#include <stdint.h>
#include <Sdt.h>

#define FADT_SIGNATURE "FACP"

#pragma pack(push)
struct GenericAddressStructure {
	uint8_t addressSpace;
	uint8_t bitWidth;
	uint8_t bitOffset;
	uint8_t accessSize;
	uint64_t address;
};

struct Fadt {
	Sdt sdt;
	uint32_t firmwareCtrl;
	uint32_t dsdt;

	// field used in ACPI 1.0; no longer in use, for compatibility only
	uint8_t  reserved;

	uint8_t  preferredPowerManagementProfile;
	uint16_t sciInterrupt;
	uint32_t smiCommandPort;
	uint8_t  acpiEnable;
	uint8_t  acpiDisable;
	uint8_t  s4biosReq;
	uint8_t  pstateControl;
	uint32_t pm1aEventBlock;
	uint32_t pm1bEventBlock;
	uint32_t pm1aControlBlock;
	uint32_t pm1bControlBlock;
	uint32_t pm2controlBlock;
	uint32_t pmTimerBlock;
	uint32_t gpe0Block;
	uint32_t gpe1Block;
	uint8_t  pm1EventLength;
	uint8_t  pm1ControlLength;
	uint8_t  pm2ControlLength;
	uint8_t  pmTimerLength;
	uint8_t  gpe0Length;
	uint8_t  gpe1Length;
	uint8_t  gpe1Base;
	uint8_t  cStateControl;
	uint16_t worstC2Latency;
	uint16_t worstC3Latency;
	uint16_t flushSize;
	uint16_t flushStride;
	uint8_t  dutyOffset;
	uint8_t  dutyWidth;
	uint8_t  dayAlarm;
	uint8_t  monthAlarm;
	uint8_t  century;

	// reserved in ACPI 1.0; used since ACPI 2.0+
	uint16_t bootArchitectureFlags;

	uint8_t  reserved2;
	uint32_t flags;

	// 12 byte structure; see below for details
	GenericAddressStructure resetReg;

	uint8_t  resetValue;
	uint8_t  reserved3[3];

	// 64bit pointers - Available on ACPI 2.0+
	uint64_t                x_firmwareControl;
	uint64_t                x_dsdt;

	GenericAddressStructure x_pm1aEventBlock;
	GenericAddressStructure x_pm1bEventBlock;
	GenericAddressStructure x_pm1aControlBlock;
	GenericAddressStructure x_pm1bControlBlock;
	GenericAddressStructure x_pm2ControlBlock;
	GenericAddressStructure x_pmTimerBlock;
	GenericAddressStructure x_gpe0Block;
	GenericAddressStructure x_gpe1Block;
};
#pragma pack(pop, 1)

Fadt* AcpiFadtLoad(uintptr_t a);