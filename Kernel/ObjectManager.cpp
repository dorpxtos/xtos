#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Log.h>
#include <MemoryAllocator.h>
#include <ObjectManager.h>

ObjDirectory* rootDirectory;
Obj* objNotFound;

ObjDirectoryNode* DirectoryObGetLastChildNode(ObjDirectory* dir) {
	if (!dir->child) {
		return (ObjDirectoryNode*)0;
	}

	ObjDirectoryNode* t = dir->child;
	while (t->next) t = t->next;
	return t;
}

Obj* DirectoryObGetLastChild(ObjDirectory* dir) {
	return DirectoryObGetLastChildNode(dir)->obj;
}

Obj* DirectoryObCreate(char* name, ObjDirectory** data) {
	return ObCreate(name, "Directory", sizeof(ObjDirectoryNode), (void**)data);
}

Obj* PointerObCreate(char* name, ObjPointer** ptr, uintptr_t to) {
	Obj* obj = ObCreate(name, "Pointer", sizeof(uintptr_t), (void**)ptr);
	*((uintptr_t*)obj->data) = to;
	return obj;
}

Obj* DirectoryObCreateChild(char* name, ObjDirectory* parent, ObjDirectory** data) {
	return ObCreateChild(name, "Directory", parent, sizeof(ObjDirectoryNode), (void**)data);
}

Obj* DirectoryObGetChildByName(ObjDirectory* dir, char* name) {
	ObjDirectoryNode* t = dir->child;

	while (t) {
		if (!strcmp(t->obj->name, name)) {
			return t->obj;
		}
		t = t->next;
	}

	return objNotFound;
}

bool DirectoryObContains(ObjDirectory* dir, char* name) {
	ObjDirectoryNode* t = dir->child;

	while (t) {
		if (!strcmp(t->obj->name, name)) {
			return true;
		}

		t = t->next;
	}

	return false;
}

int DirectoryObPrintTree(ObjDirectory* dir, int level) {
	ObjDirectoryNode* t = dir->child;

	while (t) {
		for (int i = 0; i < level + 1; i++) {
			LogPrintChar(' ');
		}

		LogPrint("%s (%s)", t->obj->name, t->obj->type);

		if (!strcmp(t->obj->type, "Directory")) {
			DirectoryObPrintTree((ObjDirectory*)t->obj->data, level + 1);
		}

		t = t->next;
	}

	return 0;
}

Obj* ObCreateChild(char* name, char* type, ObjDirectory* parent, size_t size, void** data) {
	Obj* obj = (Obj*)MemoryAllocate(sizeof(Obj));
	obj->data = MemoryAllocate(size);
	obj->dataSize = size;
	obj->name = name;
	obj->type = type;
	obj->parent = parent;

	if (parent) {
		ObjDirectoryNode* dirnode = (ObjDirectoryNode*)MemoryAllocate(sizeof(ObjDirectoryNode));
		dirnode->obj = obj;
		ObjDirectoryNode* lastChild = DirectoryObGetLastChildNode(parent);
		if (!lastChild) {
			parent->child = dirnode;
		}
		else {
			lastChild->next = dirnode;
		}
		parent->childCount++;
	}
	
	if (data) {
		*data = obj->data;
	}
	
	return obj;
}

char* ObParsePath(char* name, ObjDirectory** dir) {
	char buffer[512];
	char* b = buffer;
	char* n = name;
	ObjDirectory* cdir = rootDirectory;

	while (*n) {
		if (*n == '/') {
			*b = 0;
			Obj* o = DirectoryObGetChildByName(rootDirectory, buffer);
			if (!strcmp(o->type, "Directory")) {
				cdir = (ObjDirectory*)o->data;
			} else {
				LogPrint("not a directory: %s", o->type);
				return NULL;
			}
			b = buffer;
			n++;
		} else {
			*b++ = *n++;
		}
	}

	char* bname = strchr(name, '/') + 1;

	if ((int)bname == 1) {
		bname = name;
	}

	*dir = cdir;

	return bname;
}

Obj* ObFind(char* name) {
	ObjDirectory* dir;
	char* bname = ObParsePath(name, &dir);
	Obj* o = DirectoryObGetChildByName(dir, bname);
	return o;
}

Obj* ObCreate(char* name, char* type, size_t size, void** data) {
	ObjDirectory* cdir;
	char* bname = ObParsePath(name, &cdir);
	return ObCreateChild(bname, type, cdir, size, data);
}

void ObInit() {
	DirectoryObCreateChild("Root", NULL, &rootDirectory);
	DirectoryObCreate("Devices", NULL);
	DirectoryObCreate("Drivers", NULL);
	DirectoryObCreate("Processes", NULL);
	DirectoryObCreate("Threads", NULL);
	DirectoryObCreate("Mountpoints", NULL);
	DirectoryObCreate("Fonts", NULL);
	LogPrint("OBJ");
}