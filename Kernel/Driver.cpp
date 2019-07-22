#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Status.h>
#include <Log.h>
#include <ObjectManager.h>
#include <MemoryAllocator.h>
#include <PortableExecutable.h>
#include <Device.h>
#include <Driver.h>

DriverObject* DriverLoad(char* name) {
	DriverObject* driver;
	PeProgram* prog = PeLoad(name, PELOAD_FLAG_INKERNEL, kernelPagemap);
	DriverObCreate(name, "No Description Provided", &driver);
	uintptr_t entryPoint = RvaToAbs(prog, prog->header->addressOfEntryPoint);
	Status (*entryPointFunc)(DriverObject*, char*) = (Status(*)(DriverObject*, char*))entryPoint;
	Log("%x", entryPointFunc);
	int rv = entryPointFunc(driver, name);
	if (rv != STATUS_SUCCESS) {
		Log("Driver did not load successfully");
		return NULL;
	}
	return driver;
}

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