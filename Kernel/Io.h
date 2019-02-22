#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <DeclSpec.h>
#include <Driver.h>
#include <Device.h>
#include <File.h>

#define IOSTATUS_NONE 0
#define IOSTATUS_SUCCESS 1
#define IOSTATUS_FAILURE 2
#define IOSTATUS_NOTIMPLEMENTED 3

#define DRIVER_FUNCTION_NULL		0x00000000
#define DRIVER_FUNCTION_READ		0x00000001
#define DRIVER_FUNCTION_WRITE		0x00000002
#define DRIVER_FUNCTION_GETINFO		0x00000003
#define DRIVER_FUNCTION_SETINFO		0x00000004
#define DRIVER_FUNCTION_FS_OPEN		0x00000005
#define DRIVER_FUNCTION_FS_CLOSE	0x00000006
#define DRIVER_FUNCTION_FS_READ		0x00000007
#define DRIVER_FUNCTION_FS_WRITE	0x00000008
#define DRIVER_FUNCTION_FS_MOUNT	0x00000009
#define DRIVER_FUNCTION_FS_UNMOUNT	0x0000000A
#define DRIVER_FUNCTION_FS_SEEK		0x0000000B
#define DRIVER_FUNCTION_FS_TELL		0x0000000C

struct IoStack {
	uint32_t majorFunction;
	uint32_t minorFunction;
	uint32_t flags;
	uint32_t control;

	union {
		struct {
			uint8_t* buffer;
			uint32_t offset;
			uint32_t count;
		} read;

		struct {
			uint8_t* buffer;
			uint32_t offset;
			uint32_t count;
		} write;

		struct {
			char* name;
			int mode;
		} fsOpen;

		struct {
			uint8_t* buffer;
			uint32_t count;
		} fsRead;

		struct {
			char driveLetter;
		} fsMount;
	} parameters;

	DeviceObject* device;
	File* file;
};

struct IoRequest {
	int status;
	IoStack stack;
};

DllExport int IoCreateDevice(char*, uint32_t, size_t, DriverObject*, DeviceObject**);
DllExport int IoDestroyRequest(IoRequest*);
DllExport IoRequest* IoCreateRequest();
DllExport int IoCallDriver(DeviceObject*, IoRequest*);
void IoInit();