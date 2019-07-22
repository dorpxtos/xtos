#pragma once
#include <stddef.h>
#include <stdint.h>
#include <DeclSpec.h>
#include <Device.h>
#include <Driver.h>
#include <Utf8.h>

#pragma pack(push, 1)
struct VbeInfoStruct {
	uint8_t versionMinor;
	uint8_t versionMajor;
	uint32_t oem;   // is a 32 bit pointer to char
	uint32_t capabilities;
	uint32_t videoModes;     // is a 32 bit pointer to uint16_t
	uint16_t videoMemoryBlocks;
	uint16_t softwareRevision;
	uint32_t vendor;   // is a 32 bit pointer to char
	uint32_t productName;   // is a 32 bit pointer to char
	uint32_t productRevision;   // is a 32 bit pointer to char
};

struct VbeModeInfo {
	uint8_t pad0[18];
	uint16_t width;
	uint16_t height;
	uint8_t pad1[3];
	uint8_t bpp;
	uint8_t pad2[14];
	uint32_t framebuffer;
	uint8_t pad3[212];
};
#pragma pack(pop)

struct DisplayObject {
	int height;
	int width;
	int colors;
	int bpp;
	uint8_t* framebuffer;
	uint8_t* prebuffer;
};

struct FontMapping {
	uint32_t codepoint;
	uint32_t glyph;
};

struct FontObject {
	uint8_t** bitmap;
	FontMapping* mapping;
	int height;
	int width;
};

struct VideoLogObject {
	int row;
	int col;
	int rows;
	int cols;
	int fg;
	int bg;
	FontObject* font;
	DisplayObject* display;
};

extern bool useVideoLog;
extern VideoLogObject* videoLog;
extern DisplayObject* mainDisplay;

DllExport int VideoSetPixel(DisplayObject* display, int x, int y, int color);
DllExport int VideoPreBufferSetPixel(DisplayObject* display, int x, int y, int color);
DllExport DeviceObject* DisplayObCreate(char* name, int w, int h, int b, uintptr_t f, DriverObject* driver, DisplayObject** display);
int VideoLogPrintChar(VideoLogObject*, utf8_int32_t);
int VideoLogPrintCharUtf8(VideoLogObject*, char*);
int VideoLogClear(VideoLogObject*);
void VideoInit();