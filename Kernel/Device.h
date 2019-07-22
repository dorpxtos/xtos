#pragma once
#include <stddef.h>
#include <stdint.h>
#include <ObjectManager.h>
#include <IoResource.h>
#include <Driver.h>

#define DEVICE_TYPE_NULL			0x00000000
#define DEVICE_TYPE_KEYBOARD		0x00000001
#define DEVICE_TYPE_SERIAL_PORT		0x00000002
#define DEVICE_TYPE_VIDEO			0x00000003
#define DEVICE_TYPE_PIPE			0x00000004
#define DEVICE_TYPE_DISK			0x00000005
#define DEVICE_TYPE_VOLUME			0x00000006
#define DEVICE_TYPE_SYSTEM			0x00000007

struct DeviceObject {
	uint32_t type;
	size_t extendedSize;
	void* extendedData;
	struct IoResourceList* resources;
	struct DriverObject* driver;
	struct DeviceObject* next;
};

Obj* DeviceObCreate(char*, uint32_t, size_t, DriverObject*, DeviceObject**);