/*
Based on http://bos.asmhackers.net/docs/floppy/docs/floppy_tutorial.txt
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Idt.h>
#include <Log.h>
#include <PortIO.h>
#include <Floppy.h>

#define DISK_PARAMETER_ADDRESS 0x000FEFC7

#define FLOPPY_PRIMARY_BASE     0x03F0
#define FLOPPY_SECONDARY_BASE   0x0370
#define STATUS_REG_A            0x0000 /*PS2 SYSTEMS*/
#define STATUS_REG_B            0x0001 /*PS2 SYSTEMS*/
#define DIGITAL_OUTPUT_REG      0x0002
#define MAIN_STATUS_REG         0x0004
#define DATA_RATE_SELECT_REG    0x0004 /*PS2 SYSTEMS*/
#define DATA_REGISTER           0x0005
#define DIGITAL_INPUT_REG       0x0007 /*AT SYSTEMS*/
#define CONFIG_CONTROL_REG      0x0007 /*AT SYSTEMS*/
#define PRIMARY_RESULT_STATUS   0x0000
#define SECONDARY_RESULT_STATUS 0x0000

/*controller commands*/
#define FIX_DRIVE_DATA          0x03
#define CHECK_DRIVE_STATUS      0x04
#define CALIBRATE_DRIVE         0x07
#define CHECK_INTERRUPT_STATUS  0x08
#define FORMAT_TRACK            0x4D
#define READ_SECTOR             0x66
#define READ_DELETE_SECTOR      0xCC
#define READ_SECTOR_ID          0x4A
#define READ_TRACK              0x42
#define SEEK_TRACK              0x0F
#define WRITE_SECTOR            0xC5
#define WRITE_DELETE_SECTOR     0xC9

#pragma pack(push, 1)
struct FloppyParameters {
	uint8_t stepRateHeadUnload;
	uint8_t headLoadNdma;
	uint8_t motorDelayOff;
	uint8_t bytesPerSector;
	uint8_t sectorsPerTrack;
	uint8_t gapLength;
	uint8_t dataLength;
	uint8_t formatGapLength;
	uint8_t filler;
	uint8_t headSettleTime;
	uint8_t motorStartTime;
};
#pragma pack(pop)

FloppyParameters floppyDisk;
bool receivedFloppyInterrupt = false;
uint8_t st0;
uint8_t cylinder;
// TODO: Floppy driver
/*
extern "C" void FloppyIrq() {
	receivedFloppyInterrupt = true;
}

static void WaitForInterrupt() {
	while (!receivedFloppyInterrupt);
	receivedFloppyInterrupt = false;
}

static void CheckInterruptStatus(int base, uint8_t* status, uint8_t* cyl) {
	FloppyWriteCommand(base, CHECK_INTERRUPT_STATUS);
	WaitFloppyData(base);
	*status = inb(base + DATA_REGISTER);
	WaitFloppyData(base);
	*cyl = inb(base + DATA_REGISTER);
}

static void CalibrateDrive(int base, char drive) {
	MotorControl(drive, START)
}

void ResetFloppyController(int base, char drive) {
	outb((base + DIGITAL_OUTPUT_REG), 0x0);
	outb((base + DIGITAL_OUTPUT_REG), 0xc);
	WaitForInterrupt();
	CheckInterruptStatus(base, &st0, &cylinder);
	outb(base + CONFIG_CONTROL_REG, 0);
	ConfigureDrive(base, drive);
	CalibrateDrive(base, drive);
}
*/
void FloppyInit() {
	memcpy(&floppyDisk, (const void*) DISK_PARAMETER_ADDRESS, sizeof(FloppyParameters));
	LogPrint("FLP");
}