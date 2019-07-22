#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <DeclSpec.h>
#include <Log.h>

DllExport int PciInit() {
	Log("initializing PCI");
	return 0;
}