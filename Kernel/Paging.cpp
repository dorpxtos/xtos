/*
Based on Mintsuki's AMAZING pmm.c and vmm.c.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Multiboot.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <ObjectManager.h>
#include <Section.h>
#include <Paging.h>

PageTableEntry* initialKernelPagemap = (PageTableEntry*)0x800000;
PageTableEntry* kernelPagemap;

#define MBITMAP_FULL ((0x4000000 / PAGE_SIZE) / 32)
static size_t bitmap_full = MBITMAP_FULL;
#define BASE (0x1000000 / PAGE_SIZE)

static uint32_t* mem_bitmap;
static uint32_t initial_bitmap[MBITMAP_FULL];
static uint32_t* tmp_bitmap;

bool useHigherHalf = false;

void IdentityMapKernel(PageTableEntry* pm) {
	for (int i = 0; i < IDENTITY_MAP_SIZE; i++) {
		size_t addr = i * PAGE_SIZE;
		MapPage(pm, addr, KERNEL_MEMORY_BASE + addr, 0x03);
	}
}

PageTableEntry* CreatePagemap() {
	PageTableEntry* pm = (PageTableEntry*)PmmAlloc(1);
	Obj* so = ObFind("Sections");
	ObjDirectory* dir = (ObjDirectory*)so->data;
	ObjDirectoryNode* dirnode = dir->child;
	while (dirnode) {
		SectionObject* section = (SectionObject*)dirnode->obj->data;
		Log("SecMap [%80X -> %80X %d]", section->physicalBase, section->virtualBase, section->size);
		if (section->sectionFlags & SECTION_FLAG_ALWAYS_LOADED) {
			for (int i = 0; i < (section->size + (section->size / 2)) / PAGE_SIZE; i++) {
				MapPage(pm, section->physicalBase + i * PAGE_SIZE, section->virtualBase + i * PAGE_SIZE, section->pageFlags);
			}
		}
		dirnode = dirnode->next;
	}
	Log("pagemap=%x", pm);
	return pm;
}

/* map physaddr -> virtaddr using pd pointer */
void MapPage(PageTableEntry* pdt, size_t phys_addr, size_t virt_addr, size_t flags) {
	PageTableEntry* pd = (PageTableEntry*)((uintptr_t)pdt + KERNEL_MEMORY_BASE);

	/* Calculate the indices in the various tables using the virtual address */
	size_t i;

	size_t pd_entry = (virt_addr & ((size_t)0x3ff << 22)) >> 22;
	size_t pt_entry = (virt_addr & ((size_t)0x3ff << 12)) >> 12;

	PageTableEntry* pt;
	PageTableEntry* pthh;

	if (pd[pd_entry] & 0x1) {
		pt = (PageTableEntry*)(pd[pd_entry] & 0xfffff000);
		if (useHigherHalf) {
			pthh = (PageTableEntry*)((uintptr_t)pt + KERNEL_MEMORY_BASE);
		} else {
			pthh = pt;
		}
	}
	else {
		/* Allocate a page for the pt. */
		pt = (PageTableEntry*)PmmAlloc(1);

		if (useHigherHalf) {
			pthh = (PageTableEntry*)((uintptr_t)pt + KERNEL_MEMORY_BASE);
		}
		else {
			pthh = pt;
		}

		/* Zero page */
		for (i = 0; i < PAGE_TABLE_ENTRIES; i++) {
			/* Zero each entry */
			pthh[i] = 0;
		}

		/* Present + writable + user (0b111) */
		pd[pd_entry] = (PageTableEntry)pt | 0x07;
	}

	/* Set the entry as present and point it to the passed physical address */
	/* Also set the specified flags */
	pthh[pt_entry] = (PageTableEntry)(phys_addr | flags);
}

int UnmapPage(PageTableEntry* pd, size_t virt_addr) {
	/* Calculate the indices in the various tables using the virtual address */
	size_t pd_entry = (virt_addr & ((size_t)0x3ff << 22)) >> 22;
	size_t pt_entry = (virt_addr & ((size_t)0x3ff << 12)) >> 12;

	PageTableEntry* pt;
	PageTableEntry* pthh;

	/* Get reference to the various tables in sequence. Return -1 if one of the tables is not present,
	* since we cannot unmap a virtual address if we don't know what it's mapped to in the first place */
	if (pd[pd_entry] & 0x1) {
		pt = (PageTableEntry *)(pd[pd_entry] & 0xfffff000);
	}
	else {
		return -1;
	}

	if (useHigherHalf) {
		pthh = (PageTableEntry*)((uintptr_t)pt + KERNEL_MEMORY_BASE);
	}
	else {
		pthh = pt;
	}

	/* Unmap entry */
	pthh[pt_entry] = 0;

	return 0;
}

/* Update flags for a mapping */
int RemapPage(PageTableEntry* pd, size_t virt_addr, size_t flags) {
	/* Calculate the indices in the various tables using the virtual address */
	size_t pd_entry = (virt_addr & ((size_t)0x3ff << 22)) >> 22;
	size_t pt_entry = (virt_addr & ((size_t)0x3ff << 12)) >> 12;

	PageTableEntry* pt;
	PageTableEntry* pthh;

	/* Get reference to the various tables in sequence. Return -1 if one of the tables is not present,
	* since we cannot unmap a virtual address if we don't know what it's mapped to in the first place */
	if (pd[pd_entry] & 0x1) {
		pt = (PageTableEntry*)(pd[pd_entry] & 0xfffff000);
	}
	else {
		return -1;
	}

	if (useHigherHalf) {
		pthh = (PageTableEntry*)((uintptr_t)pt + KERNEL_MEMORY_BASE);
	}
	else {
		pthh = pt;
	}

	/* Update flags */
	pthh[pt_entry] = (pthh[pt_entry] & 0xfffff000) | flags;

	return 0;
}

/* Identity map the first 4GiB of memory, this saves issues with MMIO hardware < 4GiB later on */
/* Then use the e820 to map all the available memory (saves on allocation time and it's easier) */
/* The latter only applies to x86_64 */
void VmmInit(void) {
	kernelPagemap = (PageTableEntry*)PmmAlloc(10);
	Log("creating new pagemap");
	for (int i = 0x10; i < 0x9000000 / PAGE_SIZE; i++) {
		int addr = i * PAGE_SIZE;
		MapPage(kernelPagemap, addr, KERNEL_MEMORY_BASE + addr, 0x03);
	}
	Log("loading new pagemap");
	_asm {
		mov eax, kernelPagemap
		mov cr3, eax
	}
	Log("loaded new pagemap");
	useHigherHalf = true;
	Log("vmm initialized");
}

static int ReadBitmap(size_t i) {
	size_t which_entry = i / 32;
	size_t offset = i % 32;

	return (int)((mem_bitmap[which_entry] >> offset) & 1);
}

static void WriteBitmap(size_t i, int val) {
	size_t which_entry = i / 32;
	size_t offset = i % 32;

	if (val)
		mem_bitmap[which_entry] |= (1 << offset);
	else
		mem_bitmap[which_entry] &= ~(1 << offset);
}

static void BitmapRealloc(void) {
	size_t i;
	uint32_t *tmp;

	if (!(tmp_bitmap = (uint32_t*)MemoryAllocate((bitmap_full + 2048) * sizeof(uint32_t)))) {
		for (;;);
	}

	memcpy((void*)tmp_bitmap, (void*)mem_bitmap, bitmap_full * sizeof(uint32_t));
	for (i = bitmap_full; i < bitmap_full + 2048; i++) {
		tmp_bitmap[i] = 0xffffffff;
	}

	bitmap_full += 2048;

	tmp = tmp_bitmap;
	tmp_bitmap = mem_bitmap;
	mem_bitmap = tmp;

	MemoryFree((void*)tmp_bitmap);
}

/* Populate bitmap using e820 data. */
void PmmInit(void) {
	size_t i;

	for (i = 0; i < bitmap_full; i++) {
		initial_bitmap[i] = 0;
	}

	mem_bitmap = initial_bitmap;

	if (!(tmp_bitmap = KERNEL_MEMORY_BASE + (uint32_t*)MemoryAllocate(bitmap_full * sizeof(uint32_t)))) {
		Log("tmp_bitmap failed to init");
		for (;;);
	}

	for (i = 0; i < bitmap_full; i++)
		tmp_bitmap[i] = initial_bitmap[i];
	mem_bitmap = tmp_bitmap;


	// Multiboot memory map
	MultibootMemoryMap* memoryMap = (MultibootMemoryMap*)(KERNEL_MEMORY_BASE + multibootInfo->mmap_addr);
	Log("MM addr=%X size=%X", multibootInfo->mmap_addr, multibootInfo->mmap_length);
	
	for (i = 0; memoryMap[i].type; i++) {
		size_t j;

		Log("MM %x: type: %x addr: %80x%80x size: %80x%80x", i, memoryMap[i].type, memoryMap[i].base_addr_high, memoryMap[i].base_addr_low, memoryMap[i].len_high, memoryMap[i].len_low);

		if (memoryMap[i].base_addr_high || memoryMap[i].len_high)
			continue;

		if (memoryMap[i].base_addr_high + memoryMap[i].len_low < memoryMap[i].base_addr_high) {
			/* overflow check */
			continue;
		}

		for (j = 0; j * PAGE_SIZE < multibootInfo->mmap_length; j++) {
			size_t addr = memoryMap[i].base_addr_high + j * PAGE_SIZE;
			size_t page = addr / PAGE_SIZE;

			/* FIXME: assume the first 32 MiB of memory to be free and usable */
			if (addr < 0x2000000)
				continue;

			while (page >= bitmap_full * 32)
				BitmapRealloc();
			if (memoryMap[i].type == 1)
				WriteBitmap(page, 0);
			else
				WriteBitmap(page, 1);
		}
	}

	Log("pmm initialized");
}

/* Allocate physical memory. */
/* NOTE: THIS RETURNS THE _PHYSICAL_ ADDRESS, NOT THE VIRTUAL ONE! */
void* PmmAlloc(size_t pg_count) {
	/* Allocate contiguous free pages. */
	size_t counter = 0;
	size_t i;
	size_t start;

	for (i = BASE; i < bitmap_full * 32; i++) {
		if (!ReadBitmap(i))
			counter++;
		else
			counter = 0;
		if (counter == pg_count)
			goto found;
	}

	Log("FAILURE IN PMMALLOC!!!!!");

	return (void*)0;

found:
	start = i - (pg_count - 1);
	for (i = start; i < (start + pg_count); i++) {
		WriteBitmap(i, 1);
	}
	
	/* Return the physical address that represents the start of this physical page(s). */
	return (void*)(start * PAGE_SIZE);
}

/* Release physical memory. */
void PmmFree(void* ptr, size_t pg_count) {
	size_t start = (size_t)ptr / PAGE_SIZE;
	size_t i;

	for (i = start; i < (start + pg_count); i++) {
		WriteBitmap(i, 0);
	}
}