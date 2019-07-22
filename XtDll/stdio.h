#pragma once
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <DeclSpec.h>
#include <File.h>
#include <Printf.h>

#define EOF -1

#define putc fputc
#define getc fgetc
#define fprintf(F, X, ...) printf(X, __VA_ARGS__)

#include <File.h>