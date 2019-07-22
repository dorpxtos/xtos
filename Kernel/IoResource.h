#pragma once
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <ObjectManager.h>
#include <Device.h>

enum IoResourceType {
	EMPTY,
	PORTRANGE,
	MEMORYRANGE,
	INTERRUPT,
	DEVICEPRIVATE
};

struct IoResource {
	IoResourceType type;
	union {
		struct {
			uint32_t length;
			uint32_t minimum;
			uint32_t maximum;
		} port;
		struct {
			uint32_t length;
			uint32_t minimum;
			uint32_t maximum;
		} memory;
		struct {
			uint32_t irq;
		} interrupt;
		struct {
			uint32_t data[3];
		} devicePrivate;
	} value;
};

struct IoResourceList {
	uint32_t count;
	IoResource resources[];
};


DllExport Obj* IoResourceListCreate(int items, IoResourceList** iorl);
DllExport IoResource* IoResourceGetNextDescriptor(IoResourceList* list);
DllExport int IoResourceListLinkWithDevice(struct DeviceObject*, struct IoResourceList*);