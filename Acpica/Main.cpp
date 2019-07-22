#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <DeclSpec.h>
#include <Log.h>

DllExport int AcpiInit() {
	Log("initializing ACPI");
	Log("initialized ACPI");
	return 0;
}