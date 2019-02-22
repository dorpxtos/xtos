#pragma once
#include <stddef.h>
#include <stdint.h>
#include <AtaPio.h>
#include <File.h>

#define FILENAME_LENGTH 11
#define FILENAME_NAME_LENGTH 8
#define FILENAME_EXTENTION_LENGTH 3

enum FatType {
	FAT12,
	FAT16,
	FAT32
};

#pragma pack(push, 1)
struct Fat12Bootsector {
	uint8_t bootjmp[3];
	uint8_t oemName[8];
	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster;
	uint16_t reservedSectorCount;
	uint8_t tableCount;
	uint16_t rootEntryCount;
	uint16_t totalSectors16;
	uint8_t mediaType;
	uint16_t tableSize16;
	uint16_t sectorsPerTrack;
	uint16_t headSideCount;
	uint32_t hiddenSectorCount;
	uint32_t totalSectors32;
	uint8_t extendedSection[476];
};

struct Fat12FileEntry {
	char filename[FILENAME_LENGTH];
	uint8_t attributes;
	uint8_t reservednt;
	uint8_t creationTimeDeciseconds;
	uint16_t creationTime;
	uint16_t creationDate;
	uint16_t accessTime;
	uint16_t clusterHigh;
	uint16_t modificationTime;
	uint16_t modificationDate;
	uint16_t clusterLow;
	uint32_t size;
};
#pragma pack(pop)

struct Fat12Filesystem {
	AtaDrive* drive;
	Fat12Bootsector bootsector;
	FatType type;
	size_t filesystemSize;
	size_t fatStartSector;
	size_t fatSectors;
	size_t rootDirStartSector;
	size_t rootDirSectors;
	size_t dataStartSector;
	size_t dataSectors;
	size_t countOfClusters;
};

extern Fat12Filesystem* bootfs;

void Fat12Init();
Fat12FileEntry* Fat12OpenFile(Fat12Filesystem*, char*);
int Fat12ReadWholeFile(Fat12Filesystem*, Fat12FileEntry*, uint8_t*);