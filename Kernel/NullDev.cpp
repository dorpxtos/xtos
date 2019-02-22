#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <Device.h>
#include <Driver.h>
#include <Io.h>
#include <NullDev.h>

DriverObject* nullDriver;
DeviceObject* nullDevice;

int NullDevRead(IoStack* req) {
	return IOSTATUS_SUCCESS;
}

int NullDevWrite(IoStack* req) {
	return IOSTATUS_SUCCESS;
}

void NullDevInit() {
	DriverObCreate("Null", "Null Driver", &nullDriver);
	IoCreateDevice("Null", DEVICE_TYPE_NULL, 0, nullDriver, &nullDevice);
	DriverObRegisterFunction(nullDriver, DRIVER_FUNCTION_READ, &NullDevRead);
	DriverObRegisterFunction(nullDriver, DRIVER_FUNCTION_WRITE, &NullDevWrite);
	LogPrint("NULLDEV");
}