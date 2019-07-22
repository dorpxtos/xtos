#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <MemoryAllocator.h>
#include <Log.h>
#include <File.h>
#include <Paging.h>
#include <Section.h>
#include <PortableExecutable.h>

#define MZ_SIGNATURE 0x5A4D
#define PE_SIGNATURE 0x4550

#define IMAGE_FILE_MACHINE_I386 0x14C

#define IMAGE_FILE_DLL 0x2000
#define IMAGE_FILE_EXECUTABLE_IMAGE 0x0002

#define INITIAL_GST_SIZE 65535
#define INITIAL_LE_SIZE 65535
#define INITIAL_DS_SIZE 65535

PeExport* globalSymbolTable;
int maxGstItems;
PeProgram** loadedExecutables;
int maxLeItems;
DebugSymbol** debugSymbols;
int maxDebugSymbols;

static bool IsValidMzSignature(PeProgram* prog) {
	uint16_t* data16 = (uint16_t*)prog->data;
	return data16[0] == MZ_SIGNATURE;
}

static bool IsValidPeSignature(PeProgram* prog) {
	return prog->header->signature == PE_SIGNATURE;
}

void PePrintSections(PeProgram* prog) {
	for (int i = 0; i < prog->header->sectionCount; i++) {
		Log("%s (%x -> %x)", prog->header->sections[i].name, prog->header->sections[i].virtualAddress, prog->header->sections[i].virtualAddress + prog->header->sections[i].virtualSize);
	}
}

void PePrintHeaderInfo(PeProgram* prog) {
	Log("imageBase = %x", prog->header->imageBase);
	PePrintSections(prog);
}

int PeLoadDll(PeProgram* prog) {
	//Log("isdll");
	return 0;
}

int PeLoadExe(PeProgram* prog) {
	//Log("isexe");
	return 0;
}

uint32_t GetVirtualAddressOfSection(PeProgram* prog, char* sectionName) {
	for (int i = 0; i < prog->header->sectionCount; i++) {
		if (!strncmp(prog->header->sections[i].name, sectionName, 8)) {
			return prog->header->sections[i].virtualAddress;
		}
	}

	return -1;
}

int PeRelocate(PeProgram* prog) {
	uint16_t* reloc = (uint16_t*)(prog->data + GetVirtualAddressOfSection(prog, ".reloc"));
	BaseRelocationBlock* relocBlock = (BaseRelocationBlock*)reloc;
	uintptr_t imgbase = prog->header->imageBase;

	// Create the virtual memory
	uint8_t* vm = (uint8_t*)MemoryAllocate(prog->header->sizeOfImage);
	uintptr_t pm = (uintptr_t)vm - KERNEL_MEMORY_BASE;

	//Log("PM is %x", pm);

	// Map image
	uint8_t pf = 0x7;
	if (prog->flags & PELOAD_FLAG_INKERNEL) {
		pf = 0x3;
		SectionCreate(pm, imgbase, prog->header->sizeOfImage, pf, SECTION_FLAG_ALWAYS_LOADED);
	}

	int k;
	for (k = 0; k < (prog->header->sizeOfImage + (prog->header->sizeOfImage / 2)) / PAGE_SIZE; k++) {
		// Mapping into process pagemap
		MapPage(prog->pagemap, pm + k * PAGE_SIZE, imgbase + k * PAGE_SIZE, pf);
	}

	prog->top = imgbase + k * PAGE_SIZE;

	//Log("Size of Image: %x", prog->header->sizeOfImage);

	// Header
	memcpy(vm, prog->data, prog->header->sizeOfHeaders);

	// Sections
	for (int i = 0; i < prog->header->sectionCount; i++) {
		memcpy(vm+prog->header->sections[i].virtualAddress, prog->data+prog->header->sections[i].pointerToRawData, prog->header->sections[i].sizeOfRawData);
	}

	prog->vm = vm;

	return 0;
}

int PeReadCoffSymbols(PeProgram* prog) {
	CoffSymbol* table = (CoffSymbol*)(prog->data + prog->header->symbolTable);
	//Log("total symbols: %d", prog->header->symbolCount);

	for (int i = 0; i < prog->header->symbolCount; i++) {
		int j;

		for (j = 0; j < maxDebugSymbols; j++) {
			if (!debugSymbols[j]) goto found;
		}

		Log("!! Debug symbol table filled !!");
		return -1;

	found:
		DebugSymbol* sym = (DebugSymbol*)MemoryAllocate(sizeof(DebugSymbol));
		sym->storageClass = table[i].sclass;
		sym->section = table[i].scnum;
		sym->type = table[i].type;
		sym->value = table[i].value;
		sym->prog = prog;

		if (table[i].e.n.zeros) {
			sym->name = table[i].e.name;
		} else {
			sym->name = (char*)table[i].e.n.offset;
		}

		debugSymbols[j] = sym;

		Log("sym=%s@%x", sym->name, sym->name);
		i += table[i].numaux;
	}

	return 0;
}

char* GetExportName(PeProgram* prog, int i) {
	uint32_t* enames = (uint32_t*)RvaToAbsPhysical(prog, (uintptr_t)prog->edata[0].namePointerRva);
	return (char*)RvaToAbsPhysical(prog, (uintptr_t)enames[i]);
}

int ExportSymbol(char* name, char* ename, uintptr_t ptr) {
	int j;

	for (j = 0; j < maxGstItems; j++) {
		if (!globalSymbolTable[j].ptr) goto found;
	}

	Log("!! Export table filled !!");
	return -1;

found:
	globalSymbolTable[j].name = name;
	globalSymbolTable[j].ename = ename;
	globalSymbolTable[j].ptr = ptr;

	//Log("%d: %s: %x", j, globalSymbolTable[j].name, globalSymbolTable[j].ptr);

	return 0;
}

int PeReadExportTable(PeProgram* prog) {
	if (!prog->header->exportTable.virtualAddress) return 0;

	ExportDirectoryTable* edata = (ExportDirectoryTable*)RvaToAbsPhysical(prog, (uintptr_t)prog->header->exportTable.virtualAddress);

	prog->edata = edata;
	prog->name = strupr((char*)RvaToAbsPhysical(prog, edata[0].nameRva));

	uint32_t* exportAddressTable = (uint32_t*)RvaToAbsPhysical(prog, edata[0].exportAddressTableRva);
	uint32_t* namePointerTable = (uint32_t*)RvaToAbsPhysical(prog, edata[0].namePointerRva);
	uint16_t* ordinalTable = (uint16_t*)RvaToAbsPhysical(prog, edata[0].ordinalTableRva);

	for (int i = 0; i < edata[0].numberOfNamePointers; i++) {
		char* name = (char*)RvaToAbsPhysical(prog, namePointerTable[i]);
		uint16_t ordinal = ordinalTable[i] + 1;
		uint32_t rva = exportAddressTable[ordinal - edata[0].ordinalBase];
		ExportSymbol(name, prog->name, prog->header->imageBase + rva);
	}

	return 0;
}

int PeReadImportTable(PeProgram* prog) {
	if (!prog->header->importTable.virtualAddress) return 0;

	ImportDirectoryTable* idata = (ImportDirectoryTable*)RvaToAbsPhysical(prog, prog->header->importTable.virtualAddress);
	//Log("idata @ %x", idata);

	PeProgram* lex;

	for (int tableIndex = 0; idata[tableIndex].importLookupTableRva; tableIndex++) {
		char* name = strupr((char*)RvaToAbsPhysical(prog, idata[tableIndex].nameRva));
		
		for (int i = 0; i < maxLeItems; i++) {
			if (loadedExecutables[i]) {
				//Log("%s wants %s and found %s.", prog->name, name, loadedExecutables[i]->name);
				if (!strcmp(loadedExecutables[i]->name, name)) {
					lex = loadedExecutables[i];
					goto isLoaded;
				}
			}
		}

		int lexflags = prog->flags;
		lex = PeLoad(name, lexflags, prog->pagemap);

	isLoaded:
		uint32_t* lookupTable = (uint32_t*)RvaToAbsPhysical(prog, idata[tableIndex].importLookupTableRva);
		uint32_t* addrTable = (uint32_t*)RvaToAbsPhysical(prog, idata[tableIndex].importAddressTableRva);

		//Log("%s %x", name, lookupTable);
		
		for (int lookupTableIndex = 0; lookupTable[lookupTableIndex]; lookupTableIndex++) {
			if (lookupTable[lookupTableIndex] & 0x80000000) {
				Log("## Ordinal imports not supported ##");
			} else {
				HintNameTableEntry* hltEntry = (HintNameTableEntry*)RvaToAbsPhysical(prog, lookupTable[lookupTableIndex]);
				addrTable[lookupTableIndex] = GetGlobalSymbol(hltEntry->name, lex->name);
				//Log("%s", hltEntry->name);
			}
		}
	}

	return 0;
}

int PeRegisterExecutable(PeProgram* prog) {
	for (int i = 0; i < maxLeItems; i++) {
		if (!loadedExecutables[i]) {
			loadedExecutables[i] = prog;
			return 0;
		}
	}

	return -1;
}

int PeInitExecutable(PeProgram* prog) {
	if (PeReadCoffSymbols(prog) ||
		PeRelocate(prog) ||
		PeReadExportTable(prog) ||
		PeRegisterExecutable(prog) ||
		PeReadImportTable(prog)) return -1;
	return 0;
}

uintptr_t GetGlobalSymbol(char* name, char* ename) {
	for (int i = 0; i < maxGstItems; i++) {
		if (!strcmp(globalSymbolTable[i].name, name) && !strcmp(globalSymbolTable[i].ename, ename)) {
			return globalSymbolTable[i].ptr;
		}
	}

	return 0;
}

PeProgram* PeLoad(char* file, int flags, PageTableEntry* pagemap) {
	File* f = FileOpen(file);

	if (f == 0) {
		Log("file not found: %s", file);
		return 0;
	}

	uint8_t* data = (uint8_t*)MemoryAllocate(f->length);
	PeProgram* prog = (PeProgram*)MemoryAllocate(sizeof(PeProgram));

	FileReadAll(f, data);

	prog->flags = flags;
	prog->pagemap = pagemap;
	prog->data = data;

	if (!IsValidMzSignature(prog)) {
		Log("invalid MZ signature (%x)", *data);
		return 0;
	}

	prog->header = (PeHeader*)(prog->data + ((uint32_t*)prog->data)[0xF]);

	if (!IsValidPeSignature(prog)) {
		Log("## Invalid PE signature (%x) ##", prog->header->signature);
		return 0;
	}

	if (prog->header->machine != IMAGE_FILE_MACHINE_I386) {
		Log("## PE is not I386 ##");
		return 0;
	}

	//Log("Image built for version %d.%d", prog->header->majorOperatingSystemVersion, prog->header->minorOperatingSystemVersion);
	//PePrintHeaderInfo(prog);
	
	if (PeInitExecutable(prog)) {
		Log("## Error loading PE executable ##");
		return 0;
	}

	if (prog->header->characteristics & IMAGE_FILE_DLL) {
		int rv = PeLoadDll(prog);
		if (rv != 0) return 0;
	} else if (prog->header->characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) {
		int rv = PeLoadExe(prog);
		if (rv != 0) return 0;
	} else {
		Log("## PE unknown type ##");
		return 0;
	}

	return prog;
}

int PeRun(PeProgram* prog) {
	uintptr_t entryPoint = RvaToAbs(prog, prog->header->addressOfEntryPoint);
	int (*entryPointFunc)(short, int) = (int(*)(short, int))entryPoint;
	Log("entry: %x", entryPoint);
	int rv = entryPointFunc(0, 0);
	return rv;
}

void PeInit() {
	Log("initializing PE support");

	maxGstItems = INITIAL_GST_SIZE;
	globalSymbolTable = (PeExport*)MemoryAllocate(maxGstItems * sizeof(PeExport));
	memset(globalSymbolTable, 0, maxGstItems * sizeof(PeExport));

	maxLeItems = INITIAL_LE_SIZE;
	loadedExecutables = (PeProgram**)MemoryAllocate(maxLeItems * sizeof(PeProgram*));
	memset(loadedExecutables, 0, maxLeItems * sizeof(PeProgram*));
		
	maxDebugSymbols = INITIAL_DS_SIZE;
	debugSymbols = (DebugSymbol**)MemoryAllocate(maxLeItems * sizeof(DebugSymbol*));
	memset(debugSymbols, 0, maxDebugSymbols * sizeof(DebugSymbol*));

	Log("initialized PE support");
}