#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <Driver.h>
#include <Device.h>
#include <Io.h>

int IoCreateDevice(char* name, uint32_t type, size_t esize, DriverObject* driver, DeviceObject** dev) {
	DeviceObCreate(name, type, esize, driver, dev);
	return 0;
}

IoRequest* IoCreateRequest() {
	IoRequest* ior = (IoRequest*)MemoryAllocate(sizeof(IoRequest));
	return ior;
}

int IoDestroyRequest(IoRequest* req) {
	MemoryFree(req);
	return 0;
}

int IoCallDriver(DeviceObject* device, IoRequest* req) {
	if (!req) return -1;
	if (!device) return -2;
	if (!device->driver) return -3;
	if (!device->driver->functions) return -4;
	if (!device->driver->functions[req->stack.majorFunction]) return -5;
	req->stack.device = device;
	int(*fn)(IoStack*) = (int(*)(IoStack*))device->driver->functions[req->stack.majorFunction];
	req->status = fn(&req->stack);

	return req->status;
}

void IoInit() {
	Log("IO Manager initialized");
}