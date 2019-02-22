#pragma once
#include <stddef.h>
#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 1024

typedef uint32_t PageTableEntry;

extern PageTableEntry* kernelPagemap;

extern "C" void PageMapInit();

void* PmmAlloc(size_t);
void PmmFree(void *, size_t);
void PmmInit(void);
void MapPage(PageTableEntry*, size_t, size_t, size_t);
int UnmapPage(PageTableEntry*, size_t);
int RemapPage(PageTableEntry*, size_t, size_t);
void VmmInit(void);
