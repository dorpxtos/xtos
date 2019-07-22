#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <Cpuid.h>

bool CpuidCheckSupportECX(CpuidFeature feature) {
	uint32_t result;
	_asm {
		mov eax, 1
		cpuid
		mov result, ecx
	}
	return result & feature;
}

bool CpuidCheckSupportEDX(CpuidFeature feature) {
	uint32_t result;
	_asm {
		mov eax, 1
		cpuid
		mov result, edx
	}
	return result & feature;
}