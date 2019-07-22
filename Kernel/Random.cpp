#include <stddef.h>
#include <stdint.h>
#include <PortIo.h>
#include <Cpuid.h>
#include <Log.h>
#include <Pit.h>
#include <Random.h>

bool rdrandSupported;
int next;

void RandomSetSeed(size_t seed) {
	next = seed;
	if (rdrandSupported) {
		int rdrandSeed = RandomNumber();
		_asm {
			mov eax, rdrandSeed
			rdseed eax
		}
	}
}

size_t RandomNumber() {
	if (rdrandSupported) {
		size_t rv;
		_asm {
			rdrand eax
			mov rv, eax
		}
		next *= rv;
	}

	next += next * 1103515245 + 12345 - (int)pitTicks + inb(0x60);

	return (unsigned int)next % RAND_MAX;
}

void RandomInit() {
	Log("checking for RDRAND support");
	rdrandSupported = CpuidCheckSupportECX(CPUID_FEAT_ECX_RDRAND);
	if (rdrandSupported) {
		Log("RDRAND is supported");
	} else {
		Log("RDRAND is not supported");
	}
	Log("simulating RNG for a bit");
	int rcount = RandomNumber() * RandomNumber();
	for (int i = 0; i < rcount % 25; i++) {
		RandomSetSeed(RandomNumber() >> i);
		Log("Random[%d] = %d", i, RandomNumber());
	}
	Log("random number generator initialized");
}