#include <PortIO.h>
#include <Idt.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <Device.h>
#include <Driver.h>
#include <Io.h>
#include <AtaPio.h>

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01

#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

// Channels:
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01

// Directions:
#define      ATA_READ      0x00
#define      ATA_WRITE     0x013

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170
#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376
#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15
#define MAX_ATA_DRIVES 64

unsigned char ata_pm = 0;
unsigned char ata_ps = 0;
unsigned char ata_sm = 0;
unsigned char ata_ss = 0;

unsigned char* ide_buf = 0;

AtaDrive ataDrives[MAX_ATA_DRIVES];
DriverObject* ataPioDriver;
DeviceObject* ataPrimaryMasterDevice;

void IdeSelectDrive(unsigned char bus, unsigned char i) {
	if (bus == ATA_PRIMARY)
		if (i == ATA_MASTER)
			outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
	else
		if (i == ATA_MASTER)
			outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

void IdePrimaryIrq(InterruptRegisters* r) {

}

void IdeSecondaryIrq(InterruptRegisters* r) {

}

unsigned char IdeIdentify(unsigned char bus, unsigned char drive) {
	uint16_t io = 0;
	IdeSelectDrive(bus, drive);
	if (bus == ATA_PRIMARY) io = ATA_PRIMARY_IO;
	else io = ATA_SECONDARY_IO;
	/* ATA specs say these values must be zero before sending IDENTIFY */
	outb(io + ATA_REG_SECCOUNT0, 0);
	outb(io + ATA_REG_LBA0, 0);
	outb(io + ATA_REG_LBA1, 0);
	outb(io + ATA_REG_LBA2, 0);
	/* Now, send IDENTIFY */
	outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	Log("sent IDENTIFY");
	/* Now, read status port */
	uint8_t status = inb(io + ATA_REG_STATUS);
	if (status) {
		/* Now, poll untill BSY is clear. */
		while (inb(io + ATA_REG_STATUS) & ATA_SR_BSY != 0);

	pm_stat_read:
		status = inb(io + ATA_REG_STATUS);
		if (status & ATA_SR_ERR) {
			Log("%s%s has ERR set. Disabled.", bus == ATA_PRIMARY ? "Primary" : "Secondary", drive == ATA_PRIMARY ? " master" : " slave");
			return 0;
		}
		while (!(status & ATA_SR_DRQ)) goto pm_stat_read;
		Log("%s%s is online.", bus == ATA_PRIMARY ? "Primary" : "Secondary", drive == ATA_PRIMARY ? " master" : " slave");
		/* Now, actually read the data */
		for (int i = 0; i<256; i++) {
			*(uint16_t *)(ide_buf + i * 2) = inw(io + ATA_REG_DATA);
		}
		return 1;
	}
}

void Ide400nsDelay(unsigned short io) {
	for (int i = 0; i < 4; i++) inb(io + ATA_REG_ALTSTATUS);
}

void IdePoll(unsigned short io) {
	for (int i = 0; i< 4; i++) inb(io + ATA_REG_ALTSTATUS);

retry:;
	uint8_t status = inb(io + ATA_REG_STATUS);
	//mprint("testing for BSY\n");
	if (status & ATA_SR_BSY) goto retry;
	//mprint("BSY cleared\n");

retry2:
	status = inb(io + ATA_REG_STATUS);
	if (status & ATA_SR_ERR) {
		Log("ERR set, device failure!");
	}
	//mprint("testing for DRQ\n");
	if (!(status & ATA_SR_DRQ)) goto retry2;
	//mprint("DRQ set, ready to PIO!\n");
	return;
}

unsigned char AtaReadOne(unsigned char* buf, unsigned int lba, AtaDrive* dev) {
	//lba &= 0x00FFFFFF; // ignore topmost byte
	/* We only support 28bit LBA so far */
	uint8_t drive = dev->drive;
	uint16_t io = 0;
	switch (drive) {
	case (ATA_PRIMARY << 1 | ATA_MASTER):
		io = ATA_PRIMARY_IO;
		drive = ATA_MASTER;
		break;
	case (ATA_PRIMARY << 1 | ATA_SLAVE):
		io = ATA_PRIMARY_IO;
		drive = ATA_SLAVE;
		break;
	case (ATA_SECONDARY << 1 | ATA_MASTER):
		io = ATA_SECONDARY_IO;
		drive = ATA_MASTER;
		break;
	case (ATA_SECONDARY << 1 | ATA_SLAVE):
		io = ATA_SECONDARY_IO;
		drive = ATA_SLAVE;
		break;
	default:
		Log("unknown drive: %d (%s)", drive, dev->name);
		return 0;
	}
	//Log("io=0x%x %s", io, drive==ATA_MASTER?"Master":"Slave");
	uint8_t cmd = (drive == ATA_MASTER ? 0xE0 : 0xF0);
	uint8_t slavebit = (drive == ATA_MASTER ? 0x00 : 0x01);
	/*Log("LBA = 0x%x", lba);
	Log("LBA>>24 & 0x0f = %d", (lba >> 24)&0x0f);
	Log("(uint8_t)lba = %d", (uint8_t)lba);
	Log("(uint8_t)(lba >> 8) = %d", (uint8_t)(lba >> 8));
	Log("(uint8_t)(lba >> 16) = %d", (uint8_t)(lba >> 16));*/
	//outb(io + ATA_REG_HDDEVSEL, cmd | ((lba >> 24)&0x0f));
	outb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
	//mprint("issued 0x%x to 0x%x\n", (cmd | (lba >> 24)&0x0f), io + ATA_REG_HDDEVSEL);
	//for(int k = 0; k < 10000; k++) ;
	outb(io + 1, 0x00);
	//mprint("issued 0x%x to 0x%x\n", 0x00, io + 1);
	//for(int k = 0; k < 10000; k++) ;
	outb(io + ATA_REG_SECCOUNT0, 1);
	//mprint("issued 0x%x to 0x%x\n", (uint8_t) numsects, io + ATA_REG_SECCOUNT0);
	//for(int k = 0; k < 10000; k++) ;
	outb(io + ATA_REG_LBA0, (uint8_t)((lba)));
	//mprint("issued 0x%x to 0x%x\n", (uint8_t)((lba)), io + ATA_REG_LBA0);
	//for(int k = 0; k < 10000; k++) ;
	outb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
	//mprint("issued 0x%x to 0x%x\n", (uint8_t)((lba) >> 8), io + ATA_REG_LBA1);
	//for(int k = 0; k < 10000; k++) ;
	outb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));
	//mprint("issued 0x%x to 0x%x\n", (uint8_t)((lba) >> 16), io + ATA_REG_LBA2);
	//for(int k = 0; k < 10000; k++) ;
	outb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
	//mprint("issued 0x%x to 0x%x\n", ATA_CMD_READ_PIO, io + ATA_REG_COMMAND);

	IdePoll(io);

	for (int i = 0; i < 256; i++) {
		uint16_t data = inw(io + ATA_REG_DATA);
		*(uint16_t *)(buf + i * 2) = data;
	}
	Ide400nsDelay(io);
	return 1;
}

void AtaRead(unsigned char* buf, unsigned int lba, unsigned int numsects, AtaDrive* dev) {
	for (int i = 0; i < numsects; i++) {
		AtaReadOne(buf, lba + i, dev);
		buf += 512;
	}
}

void AtaProbe() {
	/* First check the primary bus,
	* and inside the master drive.
	*/

	if (IdeIdentify(ATA_PRIMARY, ATA_MASTER)) {
		ata_pm = 1;
		IoCreateDevice("AtaPrimaryMaster", DEVICE_TYPE_DISK, sizeof(AtaDrive), ataPioDriver, &ataPrimaryMasterDevice);
		AtaDrive* drv = ObSub(ataPrimaryMasterDevice, DeviceObject, AtaDrive);
		char* str = (char*)MemoryAllocate(40);

		/* Now, process the IDENTIFY data */
		/* Model goes from W#27 to W#46 */
		for (int i = 0; i < 40; i += 2) {
			str[i] = ide_buf[ATA_IDENT_MODEL + i + 1];
			str[i + 1] = ide_buf[ATA_IDENT_MODEL + i];
		}

		drv->drive = (ATA_PRIMARY << 1) | ATA_MASTER;
		drv->id = 0;
		drv->name = str;

		Log("device: %s", drv->name);
	}
	IdeIdentify(ATA_PRIMARY, ATA_SLAVE);
	/*ide_identify(ATA_SECONDARY, ATA_MASTER);
	ide_identify(ATA_SECONDARY, ATA_SLAVE);*/
}

IoStatus AtaPioDevRead(IoStack* req) {
	AtaDrive* drv = ObSub(ataPrimaryMasterDevice, DeviceObject, AtaDrive);
	AtaRead(req->parameters.read.buffer, req->parameters.read.offset, req->parameters.read.count, drv);
	return IOSTATUS_SUCCESS;
}

IoStatus AtaPioDevWrite(IoRequest* req) {
	return IOSTATUS_NOTIMPLEMENTED;
}

void AtaPioInit() {
	DriverObCreate("ATAPIO", "ATA-PIO Driver", &ataPioDriver);
	DriverObRegisterFunction(ataPioDriver, DRIVER_FUNCTION_READ, &AtaPioDevRead);
	DriverObRegisterFunction(ataPioDriver, DRIVER_FUNCTION_WRITE, &AtaPioDevWrite);
	AtaProbe();
	Log("AtaPio initialized");
}