#pragma once
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <DeclSpec.h>
#include <Fat12.h>
#include <Device.h>
#include <ObjectManager.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define DEFAULT_DRIVE_LETTER '$'

struct File {
	char* name;
	DeviceObject* device;
	size_t length;
	size_t position;
	void* context;
};

struct FileArguments {
	char* name;
	int mode;
};

struct VolumeObject {
	char driveLetter;
	DeviceObject* device;
	ObjPointer* mountpoint;
};

DllExport File* FileOpen(char*);
DllExport int FileClose(File*);
DllExport off_t FileSeek(File* f, off_t offset, int whence);
DllExport off_t FileTell(File*);
DllExport int FileReadAll(File*, uint8_t*);
DllExport int VolumeObCreate(char*, DeviceObject*, size_t, DriverObject*, void**);
DllExport int VolumeObMount(VolumeObject*, char);