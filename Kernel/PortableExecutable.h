#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Paging.h>

#define PELOAD_FLAG_INKERNEL 0b000001

#pragma pack(push, 1)
struct DataDirectory {
	uint32_t virtualAddress;
	uint32_t size;
};

struct PeSection {
	char name[8];
	uint32_t virtualSize;
	uint32_t virtualAddress;
	uint32_t sizeOfRawData;
	uint32_t pointerToRawData;
	uint32_t pointerToRelocations;
	uint32_t pointerToLineNumbers;
	uint16_t numberOfRelocations;
	uint16_t numberOfLineNumbers;
	uint32_t characteristics;
};

struct PeHeader {
	uint32_t signature;
	uint16_t machine;
	uint16_t sectionCount;
	uint32_t timeDateStamp;
	uint32_t symbolTable;
	uint32_t symbolCount;
	uint16_t optionalHeaderSize;
	uint16_t characteristics;

	uint16_t magic;
	uint8_t majorLinkerVersion;
	uint8_t minorLinkerVersion;
	uint32_t sizeOfCode;
	uint32_t sizeOfInitializedData;
	uint32_t sizeOfUninitializedData;
	uint32_t addressOfEntryPoint;
	uint32_t baseOfCode;
	uint32_t baseOfData;

	uint32_t imageBase;
	uint32_t sectionAlignment;
	uint32_t fileAlignment;
	uint16_t majorOperatingSystemVersion;
	uint16_t minorOperatingSystemVersion;
	uint16_t majorImageVersion;
	uint16_t minorImageVersion;
	uint16_t majorSubsystemVersion;
	uint16_t minorSubsystemVersion;
	uint32_t win32VersionValue;
	uint32_t sizeOfImage;
	uint32_t sizeOfHeaders;
	uint32_t checksum;
	uint16_t subsystem;
	uint16_t dllCharacteristics;
	uint32_t sizeOfStackReserve;
	uint32_t sizeOfStackCommit;
	uint32_t sizeOfHeapReserve;
	uint32_t sizeOfHeapCommit;
	uint32_t loaderFlags;
	uint32_t numberOfRvaAndSizes;

	DataDirectory exportTable;
	DataDirectory importTable;
	DataDirectory resourceTable;
	DataDirectory exceptionTable;
	DataDirectory certificateTable;
	DataDirectory baseReallocationTable;
	DataDirectory debug;
	DataDirectory architectureData;
	DataDirectory globalPtr;
	DataDirectory tlsTable;
	DataDirectory loadConfigTable;
	DataDirectory boundImport;
	DataDirectory importAddressTable;
	DataDirectory delayImportDescriptor;
	DataDirectory clrRuntimeHeader;
	DataDirectory nullDataDirectory;

	PeSection sections[];
};

struct BaseRelocationBlock {
	uint32_t pageRva;
	uint32_t blockSize;
};

struct ExportDirectoryTable {
	uint32_t exportFlags;
	uint32_t timeDateStamp;
	uint16_t majorVersion;
	uint16_t minorVersion;
	uint32_t nameRva;
	uint32_t ordinalBase;
	uint32_t addressTableEntries;
	uint32_t numberOfNamePointers;
	uint32_t exportAddressTableRva;
	uint32_t namePointerRva;
	uint32_t ordinalTableRva;
};

struct ImportDirectoryTable {
	uint32_t importLookupTableRva;
	uint32_t timeDateStamp;
	uint32_t forwarderChain;
	uint32_t nameRva;
	uint32_t importAddressTableRva;
};

struct HintNameTableEntry {
	uint16_t hint;
	char name[];
};

struct PeExport {
	char* ename;
	char* name;
	uint32_t ptr;
};

struct CoffSymbol {
	union {
		char name[8];
		struct {
			uint64_t zeros;
			uint64_t offset;
		} n;
	} e;
	uint64_t value;
	short scnum;
	uint16_t type;
	uint8_t sclass;
	uint8_t numaux;
};
#pragma pack(pop)

struct PeProgram {
	PeHeader* header;
	uint8_t* data;
	uint8_t* vm;
	ExportDirectoryTable* edata;
	PageTableEntry* pagemap;
	char* name;
	uintptr_t top;
	int flags;
};

struct DebugSymbol {
	char* name;
	uint32_t value;
	uint16_t section;
	uint16_t type;
	uint8_t storageClass;
	PeProgram* prog;
};

extern PeExport* globalSymbolTable;
extern int maxGstItems;
extern PeProgram** loadedExecutables;
extern int maxLeItems;
extern DebugSymbol** debugSymbols;
extern int maxDebugSymbols;

inline uintptr_t RvaToAbs(PeProgram* prog, uintptr_t addr) {
	return (uintptr_t)(prog->header->imageBase + addr);
}

inline uintptr_t RvaToAbsPhysical(PeProgram* prog, uintptr_t addr) {
	return (uintptr_t)(prog->vm + addr);
}

PeProgram* PeLoad(char*, int, PageTableEntry*);
int PeRun(PeProgram*);
int PeReadExportTable(PeProgram*);
int PeReadImportTable(PeProgram*);
int PeRegisterExecutable(PeProgram*);
int PeInitExecutable(PeProgram*);
int ExportSymbol(char*, uintptr_t);
uintptr_t GetGlobalSymbol(char*,char*);
void PeInit();