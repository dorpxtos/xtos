#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <Status.h>
#include <ObjectManager.h>
#include <Driver.h>
#include <Device.h>

extern "C" {
	/* Entry point for the driver */
	Status DriverEntry(DriverObject* driver, char* regpath) {
		Log("driver loader");

		// Successfully loaded driver
		return STATUS_SUCCESS;
	}
}