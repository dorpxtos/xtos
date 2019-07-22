#pragma once
#include <stddef.h>
#include <stdint.h>

#define RAND_MAX 2147483648

void RandomSetSeed(size_t);
size_t RandomNumber();
void RandomInit();