/*
Based on Mintsuki's AMAZING pmm.c and vmm.c.
*/

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <Multiboot.h>
#include <Paging.h>

PageTableEntry* kernelPagemap = (PageTableEntry*)0x800000;

/* map physaddr -> virtaddr using pd pointer */
void MapPage(PageTableEntry* pd, size_t phys_addr, size_t virt_addr, size_t flags) {
	/* Calculate the indices in the various tables using the virtual address */
	size_t i;

	size_t pd_entry = (virt_addr & ((size_t)0x3ff << 22)) >> 22;
	size_t pt_entry = (virt_addr & ((size_t)0x3ff << 12)) >> 12;

	PageTableEntry* pt;

	if (pd[pd_entry] & 0x1) {
		pt = (PageTableEntry*)(pd[pd_entry] & 0xfffff000);
	}
	else {
		/* Allocate a page for the pt. */
		pt = (PageTableEntry*)PmmAlloc(1);

		/* Zero page */
		for (i = 0; i < PAGE_TABLE_ENTRIES; i++) {
			/* Zero each entry */
			pt[i] = 0;
		}

		/* Present + writable + user (0b111) */
		pd[pd_entry] = (PageTableEntry)pt | 0x07;
	}

	/* Set the entry as present and point it to the passed physical address */
	/* Also set the specified flags */
	pt[pt_entry] = (PageTableEntry)(phys_addr | flags);
	return;
}

int UnmapPage(PageTableEntry* pd, size_t virt_addr) {
	/* Calculate the indices in the various tables using the virtual address */
	size_t pd_entry = (virt_addr & ((size_t)0x3ff << 22)) >> 22;
	size_t pt_entry = (virt_addr & ((size_t)0x3ff << 12)) >> 12;

	PageTableEntry* pt;

	/* Get reference to the various tables in sequence. Return -1 if one of the tables is not present,
	* since we cannot unmap a virtual address if we don't know what it's mapped to in the first place */
	if (pd[pd_entry] & 0x1) {
		pt = (PageTableEntry *)(pd[pd_entry] & 0xfffff000);
	}
	else {
		return -1;
	}

	/* Unmap entry */
	pt[pt_entry] = 0;

	return 0;
}

/* Update flags for a mapping */
int RemapPage(PageTableEntry* pd, size_t virt_addr, size_t flags) {
	/* Calculate the indices in the various tables using the virtual address */
	size_t pd_entry = (virt_addr & ((size_t)0x3ff << 22)) >> 22;
	size_t pt_entry = (virt_addr & ((size_t)0x3ff << 12)) >> 12;

	PageTableEntry* pt;

	/* Get reference to the various tables in sequence. Return -1 if one of the tables is not present,
	* since we cannot unmap a virtual address if we don't know what it's mapped to in the first place */
	if (pd[pd_entry] & 0x1) {
		pt = (PageTableEntry*)(pd[pd_entry] & 0xfffff000);
	}
	else {
		return -1;
	}

	/* Update flags */
	pt[pt_entry] = (pt[pt_entry] & 0xfffff000) | flags;

	return 0;
}

/* Identity map the first 4GiB of memory, this saves issues with MMIO hardware < 4GiB later on */
/* Then use the e820 to map all the available memory (saves on allocation time and it's easier) */
/* The latter only applies to x86_64 */
void VmmInit(void) {
	size_t i;

	for (i = 0; i < (0x10000000 / PAGE_SIZE); i++) {
		size_t addr = i * PAGE_SIZE;
		MapPage(kernelPagemap, addr, addr, 0x03);
		//LogPrint("MapPage(%x, %x, %x, %x)", kernelPagemap, addr, addr, 0x03);
	}

	LogPrint("VMM");
}


#define MBITMAP_FULL ((0x4000000 / PAGE_SIZE) / 32)
static size_t bitmap_full = MBITMAP_FULL;
#define BASE (0x1000000 / PAGE_SIZE)

static uint32_t *mem_bitmap;
static uint32_t initial_bitmap[MBITMAP_FULL];
static uint32_t *tmp_bitmap;

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

	return;
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

	return;
}

/* Populate bitmap using e820 data. */
void PmmInit(void) {
	size_t i;

	for (i = 0; i < bitmap_full; i++) {
		initial_bitmap[i] = 0;
	}

	mem_bitmap = initial_bitmap;

	if (!(tmp_bitmap = (uint32_t*)MemoryAllocate(bitmap_full * sizeof(uint32_t)))) {
		LogPrint("tmp_bitmap failed to init");
		for (;;);
	}

	for (i = 0; i < bitmap_full; i++)
		tmp_bitmap[i] = initial_bitmap[i];
	mem_bitmap = tmp_bitmap;


	// Multiboot memory map
	MultibootMemoryMap* memoryMap = (MultibootMemoryMap*)multibootInfo->mmap_addr;
	LogPrint("MM addr=%X size=%X", multibootInfo->mmap_addr, multibootInfo->mmap_length);
	
	for (i = 0; memoryMap[i].type; i++) {
		size_t j;

		LogPrint("MM %x: type: %x addr: %80x%80x size: %80x%80x", i, memoryMap[i].type, memoryMap[i].base_addr_high, memoryMap[i].base_addr_low, memoryMap[i].len_high, memoryMap[i].len_low);

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

	LogPrint("PMM");
	return;
}

/* Allocate physical memory. */
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

	return;
}