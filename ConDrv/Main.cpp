#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <Status.h>
#include <ObjectManager.h>
#include <Io.h>
#include <Driver.h>
#include <Device.h>

extern "C" {
	IoStatus ConDrvDevRead(IoStack* req) {
		return IOSTATUS_SUCCESS;
	}

	IoStatus ConDrvDevWrite(IoStack* req) {
		return IOSTATUS_SUCCESS;
	}

	/* Entry point for the driver */
	Status DriverEntry(DriverObject* driver, char* regpath) {
		driver->name = "ConDrv";
		driver->description = "Console Driver";
		DriverObRegisterFunction(driver, DRIVER_FUNCTION_READ, &ConDrvDevRead);
		DriverObRegisterFunction(driver, DRIVER_FUNCTION_WRITE, &ConDrvDevWrite);
		Log("loaded condrv: %s", driver->name);

		// Successfully loaded driver
		return STATUS_SUCCESS;
	}
}