#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <Id.h>
#include <ObjectManager.h>
#include <Device.h>
#include <IoResource.h>

Obj* IoResourceListCreate(int items, IoResourceList** iorl) {
	char name[100];
	memset(name, 0, 100);
	strcpy(name, "IoResourceLists/");
	IdGenerate(name + strlen(name));
	Obj* o = ObCreate(strdup(name), "IoResourceList", sizeof(IoResourceList) + sizeof(IoResource) * items, (void**)iorl);
	(*iorl)->count = items;
	return o;
}

IoResource* IoResourceGetNextDescriptor(IoResourceList* list) {
	for (int i = 0; i < list->count; i++) {
		if (list->resources[i].type == EMPTY) return &list->resources[i];
	}

	return NULL;
}

int IoResourceListLinkWithDevice(DeviceObject* dev, IoResourceList* iorl) {
	dev->resources = iorl;
	return 0;
}