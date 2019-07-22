/**
 * Copyright (c) 2016 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `ini.c` for details.
 */

#pragma once

#define INI_VERSION "0.1.1"

typedef struct Ini Ini;

Ini* IniLoad(const char* filename);
void        IniFree(Ini* ini);
const char* IniGet(Ini* ini, const char* section, const char* key);
//int         IniSGet(Ini* ini, const char* section, const char* key, const char* scanfmt, void* dst);