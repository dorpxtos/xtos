#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <Status.h>
#include <ObjectManager.h>
#include <Io.h>
#include <Driver.h>
#include <Device.h>
#include <PcSpeaker.h>

extern "C" {
	IoStatus BeepDevBeep(IoStack* req) {
		if (req->parameters.beep.frequency) {
			PcSpeakerPlay(req->parameters.beep.frequency);
		} else {
			PcSpeakerStop();
		}

		return IOSTATUS_SUCCESS;
	}

	/* Entry point for the driver */
	Status DriverEntry(DriverObject* driver, char* regpath) {
		driver->name = "Beep";
		driver->description = "PC Beeper Driver";
		DriverObRegisterFunction(driver, DRIVER_FUNCTION_BEEP, &BeepDevBeep);

		// Successfully loaded driver
		return STATUS_SUCCESS;
	}
}