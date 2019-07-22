#include <Log.h>
#include <Gdi.h>
#include <User.h>
#include <DeclSpec.h>
#include <UmiInit.h>

DllExport extern int I8042Init();

void UmiInit() {
	Log("initializing UMI");
	I8042Init();
	GdiKrnlInit();
	UserKrnlInit();
	Log("initialized UMI");
}