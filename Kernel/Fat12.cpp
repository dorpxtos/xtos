#include <stdint.h>
#include <string.h>
#include <AtaPio.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <ObjectManager.h>
#include <Driver.h>
#include <Device.h>
#include <Io.h>
#include <File.h>
#include <Fat12.h>

DriverObject* fat12Driver;
DeviceObject* bootfsDevice;
Fat12Filesystem* bootfs;

static void Fat12ReadSectors(Fat12Filesystem* fs, uint8_t* buffer, uint32_t lba, uint32_t count) {
	AtaRead(buffer, lba, count, fs->drive);
}

static inline int Fat12FirstSectorOfCluster(Fat12Filesystem* fs, int cluster) {
	return fs->dataStartSector + (cluster - 2) * fs->bootsector.sectorsPerCluster;
}

int Fat12DevFsRead(IoStack* req) {
	Fat12Filesystem* fs = ObSub(ObSub(req->file->device, DeviceObject, VolumeObject), VolumeObject, Fat12Filesystem);

	// TODO: Do not read the whole file
	Fat12ReadWholeFile(fs, (Fat12FileEntry*)req->file->context, req->parameters.fsRead.buffer);

	return IOSTATUS_SUCCESS;
}

int Fat12DevFsOpen(IoStack* req) {
	VolumeObject* vol = ObSub(req->device, DeviceObject, VolumeObject);
	Fat12Filesystem* fs = ObSub(vol, VolumeObject, Fat12Filesystem);
	Fat12FileEntry* fatFile = Fat12OpenFile(fs, req->parameters.fsOpen.name);

	if (!fatFile) {
		return IOSTATUS_FAILURE;
	}

	req->file->name = fatFile->filename;
	req->file->length = fatFile->size;
	req->file->position = 0;
	req->file->device = req->device;
	req->file->context = fatFile;

	return IOSTATUS_SUCCESS;
}

char* Fat12ConvertFilename(char* dst, char* src) {
	char* d = dst;
	
	memset(d, 0, 12);

	for (int i = 0; i < strlen(src); i++) {
		if (src[i] == '.') {
			for (int j = i; j < 8; j++) {
				*d++ = ' ';
			}
		}
		else {
			*d++ = src[i];
		}
	}

	return strupr(dst);
}

int Fat12CreateFilesystem(AtaDrive* drive, DeviceObject** obj, Fat12Filesystem** fs) {
	char* name = (char*)MemoryAllocate(9);
	strcpy(name, "Fat12Fs");
	name[7] = '0' + drive->id;
	DeviceObject* devobj = ObSuper(drive, DeviceObject);
	DriverObRegisterFunction(fat12Driver, DRIVER_FUNCTION_FS_OPEN, &Fat12DevFsOpen);
	DriverObRegisterFunction(fat12Driver, DRIVER_FUNCTION_FS_READ, &Fat12DevFsRead);
	VolumeObCreate(name, devobj, sizeof(Fat12Filesystem), fat12Driver, (void**)fs);
	(*fs)->drive = drive;
	return 0;
}

int Fat12DestroyFilesystem(Fat12Filesystem* fs) {
	MemoryFree(fs);
	return 0;
}

void Fat12ReadBootsector(Fat12Filesystem* fs) {
	Fat12ReadSectors(fs, (uint8_t*)&fs->bootsector, 0, 1);
	fs->filesystemSize = fs->bootsector.totalSectors16 * fs->bootsector.bytesPerSector;
	fs->fatStartSector = fs->bootsector.reservedSectorCount;
	fs->fatSectors = fs->bootsector.tableCount * fs->bootsector.tableSize16;
	fs->rootDirStartSector = fs->fatStartSector + fs->fatSectors;
	fs->rootDirSectors = (sizeof(Fat12FileEntry) * fs->bootsector.rootEntryCount + fs->bootsector.bytesPerSector - 1) / fs->bootsector.bytesPerSector;
	fs->dataStartSector = fs->rootDirStartSector + fs->rootDirSectors;
	fs->dataSectors = fs->bootsector.totalSectors16 - fs->dataStartSector;
	fs->countOfClusters = fs->dataSectors / fs->bootsector.sectorsPerCluster;
	
	if (fs->countOfClusters <= 4085) {
		fs->type = FAT12;
	}
	else if (fs->countOfClusters >= 4086 && fs->countOfClusters <= 65525) {
		fs->type = FAT16;
	}
	else {
		fs->type = FAT32;
	}
}

void Fat12PrintBootsector(Fat12Filesystem* fs) {
	LogPrint("bytesPerSector = %d\nsectorsPerCluster = %d\nreservedSectorCount = %d\ntableCount = %d\nrootEntryCount = %d\ntotalSectors16 = %d\nmediaType = %d\ntableSize16 = %d\nsectorsPerTrack = %d\nheadSideCount = %d\nhiddenSectorCount = %d\ntotalSectors32 = %d",
		fs->bootsector.bytesPerSector, fs->bootsector.sectorsPerCluster, fs->bootsector.reservedSectorCount, fs->bootsector.tableCount, fs->bootsector.rootEntryCount, fs->bootsector.totalSectors16, fs->bootsector.mediaType, fs->bootsector.tableSize16, fs->bootsector.sectorsPerTrack, fs->bootsector.headSideCount, fs->bootsector.totalSectors32);
}

void Fat12PrintMediaInfo(Fat12Filesystem* fs) {
	LogPrint("filesystemSize: %d (%d KiB)\nFAT: %x\nDIR: %x\nDAT: %x", fs->filesystemSize, fs->filesystemSize/1024, fs->fatStartSector, fs->rootDirStartSector, fs->dataStartSector);
}

Fat12FileEntry* Fat12ReadRootDirectory(Fat12Filesystem* fs, Fat12FileEntry* files) {
	Fat12ReadSectors(fs, (uint8_t*)files, fs->rootDirStartSector, fs->rootDirSectors);

	for (int i = 0; i < fs->bootsector.rootEntryCount; i++) {
		if (files[i].filename[0] == 0) break;
		//LogPrint("[%s] sector=%d", files[i].filename, Fat12FirstSectorOfCluster(fs, files[i].clusterLow));
	}

	return files;
}

Fat12FileEntry* Fat12OpenFile(Fat12Filesystem* fs, char* n) {
	char name[12];
	
	Fat12ConvertFilename(name, n);
	Fat12FileEntry* files = (Fat12FileEntry*)MemoryAllocate(sizeof(Fat12FileEntry) * fs->bootsector.rootEntryCount);
	Fat12ReadRootDirectory(fs, files);

	for (int i = 0; files[i].filename[0]; i++) {
		if (!strncmp(files[i].filename, name, FILENAME_LENGTH)) {
			Fat12FileEntry* f = (Fat12FileEntry*)MemoryAllocate(sizeof(Fat12FileEntry));
			MemoryFree(files);
			memcpy(f, &files[i], sizeof(Fat12FileEntry));
			return f;
		}
	}

	return 0;
}

int Fat12ReadFat(Fat12Filesystem* fs, uint8_t* buffer) {
	Fat12ReadSectors(fs, buffer, fs->fatStartSector, fs->fatSectors);
	return 0;
}

int Fat12ReadFileClusters(Fat12Filesystem* fs, Fat12FileEntry* f, uint8_t* buffer, size_t offset, size_t count) {
	// Active cluster info
	int cluster = f->clusterLow;

	// Buffer offset
	int bufoffset = 0;

	// Clusters read
	int read = 0;

	// FAT
	uint8_t* fat = (uint8_t*)MemoryAllocate(fs->bootsector.bytesPerSector * fs->fatSectors);

	// Read FAT
	Fat12ReadFat(fs, fat);

	// Sector read loop
	while (read < count) {
		// Read cluster only if after or at the offset
		if (read >= offset) {
			Fat12ReadSectors(fs, buffer + bufoffset, fs->dataStartSector + ((cluster-2) * fs->bootsector.sectorsPerCluster), fs->bootsector.sectorsPerCluster);
			bufoffset += fs->bootsector.bytesPerSector * fs->bootsector.sectorsPerCluster;
		}

		// Caculate next cluster
		int entoffset = cluster + (cluster / 2);
		uint16_t tableval = *(uint16_t*)(&fat[entoffset]);

		if (cluster & 0x0001)
			tableval = tableval >> 4;
		else
			tableval = tableval & 0x0FFF;

		//LogPrint("%x->%x %x", cluster, tableval, bufoffset);
		
		if (tableval == 0x000) {
			LogPrint("FAT entry is FREE, expected USED (Corrupt FAT?)");
			break;
		}
		else if (tableval == 0x001 || (tableval >= 0xFF0 && tableval <= 0xFF6)) {
			LogPrint("FAT entry is RESERVED, expected USED (Corrupt FAT?)");
			break;
		}
		else if (tableval >= 0x002 && tableval <= 0xFEF) {
			cluster = tableval;
		}
		else if (tableval == 0xFF7) {
			LogPrint("Bad FAT Sector");
			break;
		}
		else if (tableval >= 0xFF8 && tableval <= 0xFFF) {
			break;
		}

		// Next
		read++;
	}

	MemoryFree(fat);
	return read;
}

int Fat12ReadWholeFile(Fat12Filesystem* fs, Fat12FileEntry* f, uint8_t* buffer) {
	int sectstoread = (f->size + (f->size / 2)) / fs->bootsector.bytesPerSector / fs->bootsector.sectorsPerCluster;
	return Fat12ReadFileClusters(fs, f, buffer, 0, sectstoread);
}

int Fat12Mount(Fat12Filesystem* fs, char driveLetter) {
	VolumeObMount((VolumeObject*)((uintptr_t)fs - sizeof(VolumeObject)), driveLetter);
	return 0;
}

void Fat12Init() {
	DriverObCreate("FAT12", "FAT12 Filesystem Driver", &fat12Driver);
	AtaDrive* drv = ObSub(ataPrimaryMasterDevice, DeviceObject, AtaDrive);
	Fat12CreateFilesystem(drv, &bootfsDevice, &bootfs);
	Fat12Mount(bootfs, DEFAULT_DRIVE_LETTER);
	Fat12ReadBootsector(bootfs);
	/*Fat12FileEntry* f = Fat12OpenFile(bootfs, "BOOT    LST");
	uint8_t* x = (uint8_t*)MemoryAllocate(f->size);
	TextmodeClear();
	Fat12ReadWholeFile(bootfs, f, x);
	LogPrint("%s", x);*/
	LogPrint("FAT");
}