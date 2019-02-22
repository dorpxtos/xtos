#pragma once
#include <DeclSpec.h>

DllExport void* MemoryAllocate(size_t);
DllExport void MemoryFree(void*);