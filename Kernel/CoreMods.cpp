#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Log.h>
#include <TextMode.h>
#include <Paging.h>
#include <MemoryAllocator.h>
#include <PortableExecutable.h>
#include <Driver.h>
/*#include <Serial.h>
#include <Vbe.h>
#include <I8042.h>*/
#include <CoreMods.h>

bool useSerial = false;

extern PeProgram** loadedExecutables;

static void LoadKernel() {
	Log("loading kernel PE");
	PeProgram* prog = (PeProgram*)MemoryAllocate(sizeof(PeProgram));
	prog->data = (uint8_t*)(KERNEL_MEMORY_BASE + 0x100000);
	prog->vm = (uint8_t*)(KERNEL_MEMORY_BASE + 0x100000);
	prog->header = (PeHeader*)(prog->data + ((uint32_t*)prog->data)[0xF]);
	prog->header->imageBase = KERNEL_MEMORY_BASE + 0x100000;
	prog->flags = PELOAD_FLAG_INKERNEL;

	Log("loading kernel exports");
	PeReadExportTable(prog);

	Log("saving kernel into loadedExecutables");
	PeRegisterExecutable(prog);

	Log("loading kernel imports");
	PeReadImportTable(prog);

	Log("loaded kernel ok");
}

void CoreModsInit() {
	LoadKernel();

	useSerial = true;
}

void PciLoad() {
	PeProgram* prog = PeLoad("PCI.DLL", PELOAD_FLAG_INKERNEL, kernelPagemap);
	Log("PCI Kernel has been loaded into virtual memory @ 0x%x", prog->header->imageBase);
	int (*pciinit)() = (int(*)())GetGlobalSymbol("PciInit", "PCI.DLL");
	if (!(uintptr_t)pciinit) {
		Log("!! PCI.DLL!AcpiInit is NULL !!");
		_asm int 3
	}
	Log("Running PCI.DLL!PciInit (0x%x)", pciinit);
	pciinit();
}

void AcpiLoad() {
	PeProgram* prog = PeLoad("ACPI.DLL", PELOAD_FLAG_INKERNEL, kernelPagemap);
	Log("ACPI Kernel has been loaded into virtual memory @ 0x%x", prog->header->imageBase);
	int (*acpiinit)() = (int(*)())GetGlobalSymbol("AcpiInit", "ACPI.DLL");
	if (!(uintptr_t)acpiinit) {
		Log("!! ACPI.DLL!AcpiInit is NULL !!");
		_asm int 3
	}
	Log("Running ACPI.DLL!AcpiInit (0x%x)", acpiinit);
	acpiinit();
}

void UmiLoad() {
	PeProgram* prog = PeLoad("GDIKRNL.DLL", PELOAD_FLAG_INKERNEL, kernelPagemap);
	Log("UMI has been loaded into virtual memory @ 0x%x", prog->header->imageBase);
	int (*umiinit)() = (int(*)())GetGlobalSymbol("UmiInit", "GDIKRNL.DLL");
	if (!(uintptr_t)umiinit) {
		Log("!! GDIKRNL.DLL!UmiInit is NULL !!");
		_asm int 3
	}
	Log("Running GDIKRNL.DLL!UmiInit (0x%x)", umiinit);
	umiinit();
}