#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <DeclSpec.h>
#include <Paging.h>
#include <MemoryAllocator.h>
#include <Id.h>
#include <ObjectManager.h>
#include <Io.h>
#include <Log.h>
#include <Section.h>

bool autoloadSections = false;

Obj* SectionObCreate(uintptr_t physicalBase, uintptr_t virtualBase, uintptr_t size, uint8_t pageFlags, uint8_t secFlags, SectionObject** section) {
	char oname[256];
	memset(oname, 0, 256);
	strcpy(oname, "Sections/");
	IdGenerate(oname + strlen(oname));
	Obj* o = ObCreate(strdup(oname), "Section", sizeof(SectionObject), (void**)section);
	(*section)->physicalBase = physicalBase;
	(*section)->virtualBase = virtualBase;
	(*section)->size = size;
	(*section)->pageFlags = pageFlags;
	(*section)->sectionFlags = secFlags;

	if ((secFlags & SECTION_FLAG_ALWAYS_LOADED) && autoloadSections) {
		for (int i = 0; i < (size + (size / 2)) / PAGE_SIZE; i++) {
			MapPage(kernelPagemap, physicalBase + i * PAGE_SIZE, virtualBase + i * PAGE_SIZE, pageFlags);
		}
	}

	return o;
}

SectionObject* SectionCreate(uintptr_t physicalBase, uintptr_t virtualBase, uintptr_t size, uint8_t pageFlags, uint8_t secFlags) {
	SectionObject* so;
	SectionObCreate(physicalBase, virtualBase, size, pageFlags, secFlags, &so);
	return so;
}

void SectionsInit() {
	Log("creating kernel memory section");
	SectionCreate(0x10000, KERNEL_MEMORY_BASE + 0x10000, IDENTITY_MAP_SIZE * PAGE_SIZE, 0x3, SECTION_FLAG_ALWAYS_LOADED);
	autoloadSections = true;
	SectionCreate(0, 0, 0x10000, 0x3, SECTION_FLAG_ALWAYS_LOADED);
	Log("sections initialized");
}