#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ObjectManager.h>
#include <MemoryAllocator.h>
#include <Driver.h>
#include <Device.h>

Obj* DeviceObCreate(char* name, uint32_t type, size_t esize, DriverObject* driver, DeviceObject** dev) {
	char* oname = (char*)MemoryAllocate(64);
	strcpy(oname, "Devices/");
	strcpy(oname + strlen("Devices/"), name);
	Obj* o = ObCreate(oname, "Device", sizeof(DeviceObject) + esize, (void**)dev);
	(*dev)->type = type;
	(*dev)->driver = driver;
	(*dev)->extendedSize = esize;
	(*dev)->extendedData = (void*)((char*)(*dev) + sizeof(DeviceObject));

	DeviceObject* cdev = driver->device;

	if (cdev) {
		while (cdev->next) {
			cdev = cdev->next;
		}
		cdev->next = *dev;
	} else {
		driver->device = *dev;
	}

	return o;
}