#include <stddef.h>
#include <stdint.h>
#include <TextMode.h>
#include <Idt.h>
#include <Log.h>
#include <Pit.h>
#include <Paging.h>
#include <Gdt.h>
#include <Multiboot.h>
#include <AtaPio.h>
#include <Floppy.h>
#include <Fat12.h>
#include <PortableExecutable.h>
#include <CoreMods.h>
#include <Task.h>
#include <Syscalls.h>
#include <ObjectManager.h>
#include <Handle.h>
#include <NullDev.h>
#include <Io.h>
#include <Video.h>

MultibootInfo* multibootInfo;

extern "C" void KernelMain(MultibootInfo* multibootData, uint32_t multibootMagic) {
	TextmodeClear();
	LogPrint("XtOS");
	
	if (multibootMagic != MULTIBOOT_BOOTLOADER_MAGIC) {
		LogPrint("This kernel must be booted from a multiboot bootloader (Magic: %80X)", multibootMagic);
		_asm {
			cli
			hlt
		}
	}

	multibootInfo = multibootData;

	PageMapInit();
	PmmInit();
	VmmInit();
	ObInit();
	IoInit();
	HandlesInit();
	IdtInit();
	GdtInit();
	VideoInit();
	PitInit();
	//FloppyInit();
	AtaPioInit();
	Fat12Init();
	PeInit();
	CoreModsInit();
	NullDevInit();
	LogDevInit();
	SyscallsInit();
	TaskInit();
	DirectoryObPrintTree(rootDirectory, 0);
	LogPrint("END");
	schedulerEnabled = true;

	while (1) {}
}