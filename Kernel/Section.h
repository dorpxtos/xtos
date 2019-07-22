#pragma once
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <ObjectManager.h>

#define SECTION_FLAG_ALWAYS_LOADED (1>>0)

struct SectionObject {
	uintptr_t physicalBase;
	uintptr_t virtualBase;
	uintptr_t size;
	uint8_t pageFlags;
	uint8_t sectionFlags;
};

Obj* SectionObCreate(uintptr_t physicalBase, uintptr_t virtualBase, uintptr_t size, uint8_t pageFlags, uint8_t secFlags, SectionObject** section);
SectionObject* SectionCreate(uintptr_t physicalBase, uintptr_t virtualBase, uintptr_t size, uint8_t pageFlags, uint8_t secFlags);
void SectionsInit();