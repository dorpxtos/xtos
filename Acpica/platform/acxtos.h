#ifndef _ACXTOS_H
#define _ACXTOS_H

#include <Log.h>

#define ACPI_MACHINE_WIDTH      32
#define ACPI_USE_NATIVE_DIVIDE
#define ACPI_USE_NATIVE_MATH64

#ifdef ACPI_DEFINE_ALTERNATE_TYPES
/*
 * Types used only in (Linux) translated source, defined here to enable
 * cross-platform compilation (i.e., generate the Linux code on Windows,
 * for test purposes only)
 */
typedef int                             s32;
typedef unsigned char                   u8;
typedef unsigned short                  u16;
typedef unsigned int                    u32;
typedef COMPILER_DEPENDENT_UINT64       u64;
#endif

#define ACPI_USE_ALTERNATE_PROTOTYPE_AcpiOsPrintf
#define AcpiOsPrintf LogPrint

#endif