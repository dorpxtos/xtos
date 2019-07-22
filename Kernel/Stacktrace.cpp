#include <stddef.h>
#include <stdint.h>
#include <Log.h>
#include <PortableExecutable.h>
#include <Stacktrace.h>

struct Stackframe {
	struct Stackframe* ebp;
	size_t eip;
};

void Stacktrace(size_t maxLevels) {
	Stackframe* stk;
	_asm mov stk, ebp
	LogPrint("Stacktrace!");
	for (size_t frame = 0; stk && frame < maxLevels; ++frame) {
		char* filename;
		char* symname;
		PeExport* recordHolder;
		size_t recordValue = 0xFFFFFFFF;
		for (int i = 0; i < maxGstItems; i++) {
			PeExport* e = &globalSymbolTable[i];
			if (stk->eip >= e->ptr) {
				size_t diff = stk->eip - e->ptr;
				if (recordValue > diff) {
					recordValue = diff;
					recordHolder = e;
				}
			}
		}
		if (recordHolder) {
			filename = recordHolder->ename;
			symname = recordHolder->name;
		} else {
			filename = "??";
			symname = "??";
		}
		LogPrint("\t0x%80X %s!%s+%x", stk->eip, filename, symname, recordValue);
		stk = stk->ebp;
	}
}