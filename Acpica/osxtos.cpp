#include <MemoryAllocator.h>
#include <Task.h>
extern "C" {
	#include "acpi.h"
	#include "accommon.h"
	#include "acpiosxf.h"

	ACPI_STATUS AcpiOsInitialize() {
		return AE_OK;
	}

	ACPI_STATUS AcpiOsTerminate() {
		return AE_OK;
	}

	void* AcpiOsAllocate(ACPI_SIZE Size) {
		return MemoryAllocate(Size);
	}

	void AcpiOsFree(void* Memory) {
		MemoryFree(Memory);
	}

	ACPI_THREAD_ID AcpiOsGetThreadId() {
		return currentThread->id;
	}

	ACPI_STATUS AcpiOsCreateSemaphore(UINT32 MaxUnits, UINT32 InitialUnits, ACPI_SEMAPHORE* OutHandle) {
		return AE_OK;
	}

	ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE Handle) {
		return AE_OK;
	}

	ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units, UINT16 Timeout) {
		return AE_OK;
	}

	ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE Handle, UINT32 Units) {
		return AE_OK;
	}

	ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK* OutHandle) {
		return AE_OK;
	}

	void AcpiOsDeleteLock(ACPI_HANDLE Handle) {

	}

	ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK Handle) {
		return AE_OK;
	}

	void AcpiOsReleaseLock(ACPI_SPINLOCK Handle, ACPI_CPU_FLAGS Flags) {

	}
}