#pragma once
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <ObjectManager.h>

#define MAX_DRIVER_FUNCTIONS 128

struct DriverObject {
	char* name;
	char* description;
	struct DeviceObject* device;
	void* functions[MAX_DRIVER_FUNCTIONS];
};

DllExport DriverObject* DriverLoad(char* name);
DllExport Obj* DriverObCreate(char*, char*, DriverObject**);
DllExport int DriverObRegisterFunction(DriverObject*, uint32_t, void*);
