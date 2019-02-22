#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Fat12.h>
#include <Device.h>
#include <Driver.h>
#include <Io.h>
#include <ObjectManager.h>
#include <MemoryAllocator.h>
#include <Log.h>
#include <File.h>

File* FileAllocate() {
	return (File*)MemoryAllocate(sizeof(File));
}

File* FileOpen(char* name) {
	char mpname[16];
	char driveLetter = DEFAULT_DRIVE_LETTER;

	if (name[1] == ':') {
		driveLetter = name[0];
	}
	
	memset(mpname, 0, 16);
	strcpy(mpname, "Mountpoints/");
	mpname[strlen(mpname)] = driveLetter;
	mpname[strlen(mpname)] = 0;

	Obj* mptr = ObFind(mpname);
	if (mptr && mptr != objNotFound) {
		VolumeObject* vol = (VolumeObject*)PointerObRead(mptr);

		DeviceObject* dev = ObSuper(vol, DeviceObject);
		File* f = FileAllocate();

		IoRequest* ior = IoCreateRequest();
		ior->stack.majorFunction = DRIVER_FUNCTION_FS_OPEN;
		ior->stack.parameters.fsOpen.name = name + (name[1]==':'?2:0);
		ior->stack.file = f;

		signed int sc = IoCallDriver(dev, ior);
		
		if (sc < 0) {
			LogPrint("## FileOpen: Error %d ##", -sc);
			IoDestroyRequest(ior);
			return NULL;
		}

		if (ior->status == IOSTATUS_SUCCESS) {
			IoDestroyRequest(ior);
			return f;
		}

		IoDestroyRequest(ior);
		return NULL;
	}
	
	LogPrint("## Could not find '%s' ##", mpname);
	return NULL;
}

int FileClose(File* f) {
	return 0;
}

int FileRead(File* f, uint8_t* buffer, size_t count) {
	DeviceObject* dev = f->device;

	IoRequest* ior = IoCreateRequest();
	ior->stack.majorFunction = DRIVER_FUNCTION_FS_READ;
	ior->stack.parameters.fsRead.count = count;
	ior->stack.parameters.fsRead.buffer = buffer;
	ior->stack.file = f;
	IoCallDriver(dev, ior);
	IoDestroyRequest(ior);
	
	if (ior->status == IOSTATUS_SUCCESS) {
		f->position += count;
		return count;
	}

	IoDestroyRequest(ior);
	return -1;
}

int FileReadAll(File* f, uint8_t* buffer) {
	int rv = FileRead(f, buffer, f->length);
	return rv;
}

/*
Structure:
Header
Device
Volume
Filesystem
*/
int VolumeObCreate(char* name, DeviceObject* driveDev, size_t extendedSize, DriverObject* driver, void** obj) {
	DeviceObject* dev;
	IoCreateDevice(name, DEVICE_TYPE_VOLUME, sizeof(VolumeObject) + extendedSize, driver, &dev);
	VolumeObject* vol = ObSub(dev, DeviceObject, VolumeObject);
	vol->device = driveDev;
	*obj = (void*)((uintptr_t)vol + sizeof(VolumeObject));
	return 0;
}

int VolumeObMount(VolumeObject* vol, char driveLetter) {
	char mpname[16];
	memset(mpname, 0, 16);
	strcpy(mpname, "Mountpoints/");
	mpname[strlen(mpname)] = driveLetter;
	mpname[strlen(mpname)] = 0;
	vol->driveLetter = driveLetter;

	PointerObCreate(strdup(mpname), &vol->mountpoint, (uintptr_t)vol);
	return 0;
}