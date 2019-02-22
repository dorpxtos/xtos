#pragma once
#include <stddef.h>
#include <stdint.h>
#include <Handle.h>
#include <Access.h>
#include <DeclSpec.h>

#define ObSuper(x, t) (t*)((uintptr_t)x - sizeof(t))
#define ObSub(x, t, d) (d*)((uintptr_t)x + sizeof(t))
#define ObGetHeader(x) ObSuper(x, Obj)
#define PointerObRead(x) ((Obj*)*((uintptr_t*)x->data))

struct ObjDirectory;
struct ObjDirectoryNode;

struct Obj {
	char* name;
	char* type;
	ObjDirectory* parent;
	size_t pointerCount;
	size_t dataSize;
	void* data;
};

struct ObjDirectory {
	int childCount;
	ObjDirectoryNode* child;
};

struct ObjDirectoryNode {
	Obj* obj;
	struct ObjDirectoryNode* next;
};

typedef uintptr_t* ObjPointer;

extern ObjDirectory* rootDirectory;
extern Obj* objNotFound;

void ObInit();
DllExport Obj* ObCreateChild(char*, char*, ObjDirectory*, size_t, void**);
DllExport Obj* ObCreate(char*, char*, size_t, void**);
DllExport Obj* PointerObCreate(char*, ObjPointer**, uintptr_t);
DllExport Obj* DirectoryObCreate(char*, ObjDirectory**);
DllExport Obj* ObFind(char*);
DllExport int DirectoryObPrintTree(ObjDirectory*, int);
