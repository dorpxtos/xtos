#include <stddef.h>
#include <stdint.h>
#include <PortIO.h>
#include <PcSpeaker.h>

void PcSpeakerPlay(uint32_t freq) {
	// Set the PIT to the desired frequency
	uint32_t div = 1193180 / freq;
	outb(0x43, 0xb6);
	outb(0x42, (uint8_t)(div));
	outb(0x42, (uint8_t)(div >> 8));

	// Play the sound using the PC speaker
	uint8_t tmp = inb(0x61);
	if (tmp != (tmp | 3)) {
		outb(0x61, tmp | 3);
	}
}

void PcSpeakerStop() {
	uint8_t tmp = inb(0x61) & 0xFC;
	outb(0x61, tmp);
}