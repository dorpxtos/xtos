
/*
 * Lux ACPI Implementation
 * Copyright (C) 2018 by Omar Mohammad
 */

/* lux-specific OS functions */

#pragma once

#include <stdint.h>
#include <stddef.h>

#define acpi_printf		kprintf

#define acpi_panic(...)	LogPrint(__VA_ARGS__); while(1);

typedef size_t acpi_lock_t;



