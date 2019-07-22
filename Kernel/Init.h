#pragma once

#define CONFIG_FILE "XTOS.INI"

struct KernelConfig {
	Ini* ini;
	char* initExecutableName;
};

extern KernelConfig kconfig;

void InitSystem();