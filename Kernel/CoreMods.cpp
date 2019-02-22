#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <TextMode.h>
#include <MemoryAllocator.h>
#include <PortableExecutable.h>
/*#include <Serial.h>
#include <Vbe.h>
#include <I8042.h>*/
#include <CoreMods.h>

bool useSerial = false;

extern PeProgram** loadedExecutables;

static void LoadKernel() {
	LogPrint("Loading kernel PE");
	PeProgram* prog = (PeProgram*)MemoryAllocate(sizeof(PeProgram));
	prog->data = (uint8_t*)0x100000;
	prog->header = (PeHeader*)(prog->data + ((uint32_t*)prog->data)[0xF]);

	LogPrint("Loading kernel exports");
	PeReadExportTable(prog);

	LogPrint("Saving kernel into loadedExecutables");
	PeRegisterExecutable(prog);

	LogPrint("Loading kernel imports");
	PeReadImportTable(prog);

	LogPrint("Loaded kernel ok");
}

void CoreModsInit() {
	LoadKernel();
	/*SerialInit();
	//I8042Init();
	VbeInit();*/

	useSerial = true;
}