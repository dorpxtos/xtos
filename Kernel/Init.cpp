#include <stddef.h>
#include <stdint.h>
#include <PortIO.h>
#include <Log.h>
#include <Task.h>
#include <Driver.h>
#include <Ini.h>
#include <Init.h>

KernelConfig kconfig;
Process* initProc;

void InitConfig() {
	kconfig.ini = IniLoad(CONFIG_FILE);
	kconfig.initExecutableName = (char*)IniGet(kconfig.ini, "Init", "Exec");
	Log("Configuration loaded");
}

void InitDrivers() {
	char buffer[128];
	int bufferi = 0;
	char* drptr = (char*)IniGet(kconfig.ini, "Drivers", "LoadDrivers");
	while (1) {
		if (*drptr == ',' || *drptr == 0) {
			buffer[bufferi] = 0;
			DriverLoad(buffer);
			Log("Loaded driver: %s", buffer);
			bufferi = 0;

			if (*drptr == 0) {
				break;
			}
		} else {
			buffer[bufferi++] = *drptr;
		}
		drptr++;
	}
	Log("Initialized drivers");
}

void InitUserspace() {
	initProc = TaskNewProcess(kconfig.initExecutableName);
	currentProcess = initProc;
	currentThread = initProc->threads[0];
	Log("successfully initialized kernel :-)");
	schedulerEnabled = true;

	while (1) {
		//Log("Stalling at end of KernelMain()! Attempting to trigger the Task Manager...");
		EnableInterrupts();
		outb(0x20, 0x20);
		outb(0xA0, 0x20);
	}
}

void InitSystem() {
	InitConfig();
	InitDrivers();
	InitUserspace();
}