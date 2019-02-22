/*
Based on Mintsuki's AMAZING klib.c.
*/

#include <Paging.h>
#include <MemoryAllocator.h>

struct AllocMetadata {
	size_t pages;
	size_t size;
};

void* MemoryAllocate(size_t size) {
	size_t page_count = size / PAGE_SIZE;
	char* ptr;
	size_t i;
	AllocMetadata* metadata;

	if (size % PAGE_SIZE) page_count++;

	ptr = (char*)PmmAlloc(page_count + 1);

	if (!ptr) {
		return (void*)0;
	}

	metadata = (AllocMetadata*)ptr;
	ptr += PAGE_SIZE;

	metadata->pages = page_count;
	metadata->size = size;

	// Zero pages.
	for (i = 0; i < (page_count * PAGE_SIZE); i++) {
		ptr[i] = 0;
	}

	return (void*)ptr;
}

void MemoryFree(void* ptr) {
	AllocMetadata* metadata = (AllocMetadata*)((size_t)ptr - PAGE_SIZE);

	PmmFree((void*)metadata, metadata->pages + 1);
}