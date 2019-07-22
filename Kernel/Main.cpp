#include <stddef.h>
#include <stdint.h>
#include <x86.h>
#include <Utf8.h>
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
#include <FastSyscalls.h>
#include <SSE.h>
#include <Random.h>
#include <Id.h>
#include <Section.h>
#include <PortIO.h>
#include <Stacktrace.h>
#include <Ini.h>
#include <Init.h>
#include <Driver.h>

MultibootInfo* multibootInfo;

extern "C" void KernelMain(uintptr_t multibootData, uint32_t multibootMagic) {
	if (multibootMagic != MULTIBOOT_BOOTLOADER_MAGIC) {
		LogPrint("This kernel must be booted from a multiboot bootloader (Magic: %80X)", multibootMagic);
		_asm {
			cli
			hlt
		}
	}

	multibootInfo = (MultibootInfo*)multibootData;
	TextmodeClear();
	LogPrint("XtOS %x", multibootInfo);
	PmmInit();
	SSEInit();
	ObInit();
	IoInit();
	HandlesInit();
	IdtInit();
	GdtInit();
	VmmInit();
	RandomInit();
	SectionsInit();
	VideoInit();
	PitInit();
	//FloppyInit();
	AtaPioInit();
	Fat12Init();
	PeInit();
	CoreModsInit();
	PciLoad();
	AcpiLoad();
	NullDevInit();
	LogDevInit();
	SyscallsInit();
	FastSyscallsInit();
	UmiLoad();
	DriverLoad("CONDRV.SYS");
	DirectoryObPrintTree(rootDirectory, 0);
	Ini* releaseini = IniLoad("RELEASE.INI");
	if (releaseini) {
		LogPrint("%s %s\n%s", IniGet(releaseini, "release", "product"), IniGet(releaseini, "release", "version"), IniGet(releaseini, "release", "copyright"));
		IniFree(releaseini);
	} else {
		Log("error loading RELEASE.INI");
	}
	utf8_int32_t cp;
	/*LogPrint("╔══════════════════════════════════════════╗\n\
║                                          ║\n\
║   • ‘single’ and “double” quotes         ║\n\
║                                          ║\n\
║   • Curly apostrophes : “We’ve been here”║\n\
║                                          ║\n\
║   • Latin - 1 apostrophe and accents : '´║\n\
║                                          ║\n\
║   • ‚deutsche‘ „Anführungszeichen“       ║\n\
║                                          ║\n\
║   • †, ‡, ‰, •, 3–4, —, −5 / +5, ™, …    ║\n\
║                                          ║\n\
║   • ASCII safety test : 1lI | , 0OD, 8B  ║\n\
║                      ╭─────────╮         ║\n\
║   • the euro symbol :│ 14.95 € │         ║\n\
║                      ╰─────────╯         ║\n\
╚══════════════════════════════════════════╝");*/
	TaskInit();
	InitSystem();
	while (1);
}