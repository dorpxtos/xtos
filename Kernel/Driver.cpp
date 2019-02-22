#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ObjectManager.h>
#include <MemoryAllocator.h>
#include <Device.h>
#include <Driver.h>

Obj* DriverObCreate(char* name, char* description, DriverObject** driver) {
	char* oname = (char*)MemoryAllocate(64);
	strcpy(oname, "Drivers/");
	strcpy(oname + strlen("Drivers/"), name);
	Obj* o = ObCreate(oname, "Driver", sizeof(DriverObject), (void**)driver);
	(*driver)->name = name;
	(*driver)->description = description;
	return o;
}

int DriverObRegisterFunction(DriverObject* driver, uint32_t num, void* func) {
	if (!driver) return -1;
	if (!driver->functions) return -2;
	if (num > MAX_DRIVER_FUNCTIONS) return -3;

	driver->functions[num] = func;

	return 0;
}