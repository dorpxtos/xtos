#include <stdint.h>
#include <string.h>
#include <MemoryAllocator.h>
#include <Random.h>
#include <Log.h>
#include <Id.h>

extern "C" {
	char* IdGenerate(char* buffer) {
		uint32_t randHi = RandomNumber();
		uint32_t randLo = RandomNumber();
		buffer[0] = '{';
		itoa(randHi, buffer + strlen(buffer), 16);
		buffer[8] = '-';
		itoa(randLo, buffer + strlen(buffer), 16);
		buffer[strlen(buffer)] = '}';
		return buffer;
	}
}