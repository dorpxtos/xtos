#pragma once
#include <DeclSpec.h>

DllImport int SerialPrintChar(char);
DllImport int SerialPrintString(char*);
DllImport int SerialIsReady();
DllImport int SerialInit();