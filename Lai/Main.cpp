#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <DeclSpec.h>
#include <Log.h>

int DllMain() {
	Log("Lai is not meant to be executed.");
	return -1;
}