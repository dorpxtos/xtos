#include <stddef.h>
#include <stdint.h>
#include <PortIO.h>
#include <PciDevice.h>
#include <Pci.h>

uint16_t PciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint32_t tmp = 0;

	address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	outl(0xCF8, address);
	tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return tmp;
}

uint16_t PciCheckVendor(uint8_t bus, uint8_t slot) {
	uint16_t vendor, device;

	if ((vendor = PciConfigReadWord(bus, slot, 0, 0)) != 0xFFFF) {
		device = PciConfigReadWord(bus, slot, 0, 2);
	}

	return vendor;
}


void CheckFunction(uint8_t bus, uint8_t device, uint8_t fn) {

}

void CheckDevice(uint8_t bus, uint8_t device) {
	uint8_t fn = 0;
	uint16_t vendor = PciCheckVendor(bus, device);
	if (vendor == 0xFFFF) return;
	CheckFunction(bus, device, fn);
}

void CheckBus(uint8_t bus) {
	for (uint8_t dev = 0; dev < 32; dev++) {
		CheckDevice(bus, dev);
	}
}