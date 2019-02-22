#pragma once
#include <stddef.h>
#include <stdint.h>

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
};

struct FontObject {
	uint8_t* bitmap;
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

void VideoInit();
int VideoLogPrintChar(VideoLogObject*, char);