#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct AccessRule {
	char* name;
	bool read;
	bool write;
	bool execute;
	bool destroy;
	bool special;
};

typedef AccessRule* AccessPolicy;