#include <stdint.h>
#include <PortIO.h>
#include <Pic.h>

#define MASTER_COMMAND 0x20
#define SLAVE_COMMAND 0xA0
#define MASTER_DATA 0x21
#define SLAVE_DATA 0xA1

enum Pic {
	MASTER,
	SLAVE
};

enum PicCommand {
	INIT = 0x11,
	EOI = 0x20
};

void PicSendCommand(Pic controller, PicCommand command) {
	outb(controller == MASTER ? MASTER_COMMAND : SLAVE_COMMAND, (uint8_t)command);
}

void PicSendData(Pic controller, uint8_t data) {
	outb(controller == MASTER ? MASTER_DATA : SLAVE_DATA, (uint8_t)data);
}


uint8_t PicReadMask(Pic controller) {
	return inb(controller == MASTER ? MASTER_DATA : SLAVE_DATA);
}

int PicRemap(uint8_t masterVector, uint8_t slaveVector) {
	// Save masks
	uint8_t masterMask = PicReadMask(MASTER);
	uint8_t slaveMask = PicReadMask(SLAVE);

	// Initialize
	PicSendCommand(MASTER, INIT);
	PicSendCommand(SLAVE, INIT);

	// Load vector offsets
	PicSendData(MASTER, masterVector);
	PicSendData(SLAVE, slaveVector);
	
	// Tell Master PIC that Slave PIC at IRQ2
	PicSendData(MASTER, 4);

	// Tell Slave PIC its cascade identity
	PicSendData(SLAVE, 2);

	// Enter MCS-80/95 mode on both PICs
	PicSendData(MASTER, 1);
	PicSendData(SLAVE, 1);

	// Restore masks
	PicSendData(MASTER, masterMask);
	PicSendData(SLAVE, slaveMask);

	// Success!
	return 0;
}

void PicDisableController(Pic controller) {
	PicSendData(controller, 0xFF);
}

void PicDisable() {
	PicDisableController(MASTER);
	PicDisableController(SLAVE);
}

void PicSendEoi(uint8_t irq) {
	if (irq >= 40) PicSendCommand(SLAVE, EOI);
	PicSendCommand(MASTER, EOI);
}