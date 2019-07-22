#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Utf8.h>
#include <Colors.h>
#include <Log.h>
#include <Paging.h>
#include <Io.h>
#include <ObjectManager.h>
#include <MemoryAllocator.h>
#include <Device.h>
#include <Driver.h>
#include <TextMode.h>
#include <Bochs.h>
#include <Section.h>
#include <Video.h>

#include "InternalFont.h"

#define VBEMODEINFO_PTR 0x9000

extern "C" extern void SetVideo();

DriverObject* vgaDriver;
DriverObject* videoLogDriver;
DisplayObject* mainDisplay;
FontObject* internalFont;
VideoLogObject* videoLog;

bool useVideoLog = false;

static inline int pow2(int x) {
	int rv = 0;
	for (int i = x; i >= 0; i--) {
		rv *= x;
	}
	return rv;
}

DeviceObject* DisplayObCreate(char* name, int w, int h, int b, uintptr_t f, DriverObject* driver, DisplayObject** display) {
	DeviceObject* dev;
	IoCreateDevice(name, DEVICE_TYPE_VIDEO, sizeof(DisplayObject), driver, &dev);
	*display = ObSub(dev, DeviceObject, DisplayObject);
	(*display)->width = w;
	(*display)->height = h;
	(*display)->bpp = b;
	(*display)->colors = pow2(b);
	(*display)->framebuffer = (uint8_t*)f;
	int size = w * h * (b / 8);
	SectionCreate((uintptr_t)((*display)->framebuffer), (uintptr_t)((*display)->framebuffer), size, 0x3, SECTION_FLAG_ALWAYS_LOADED);
	(*display)->prebuffer = (uint8_t*)MemoryAllocate(h * w * (b / 8));

	return dev;
}

int VideoSetPixel(DisplayObject* display, int x, int y, int color) {
	if (!display) return -1;
	
	if (display->bpp == 24) {
		int p = (y * display->width + x) * (display->bpp / 8);
		display->framebuffer[p] = color & 0xFF;
		display->framebuffer[p+1] = (color >> 8) & 0xFF;
		display->framebuffer[p+2] = (color >> 16) & 0xFF;
	}

	return 0;
}

int VideoPreBufferSetPixel(DisplayObject* display, int x, int y, int color) {
	if (!display) return -1;

	if (display->bpp == 24) {
		int p = (y * display->width + x) * (display->bpp / 8);
		display->prebuffer[p] = color & 0xFF;
		display->prebuffer[p + 1] = (color >> 8) & 0xFF;
		display->prebuffer[p + 2] = (color >> 16) & 0xFF;
	}

	return 0;
}

int VideoPutGlyph(DisplayObject* display, int c, int x, int y, int fg, int bg, uint8_t font[][8], int wi, int hi) {
	// For every row in the font height
	for (char i = 0; i < hi; i++) {
		// Let fl = the bitmap of the character that is going to be written
		unsigned short fl = font[c][i];
		// For every column in the font width
		for (char j = 0; j < wi; j++) {
			// Let fc = The pixel of the bitmap that is going to be written
			unsigned short fc = (fl >> j) & 1;

			// Check if the pixel is on or not
			if (fc) {
				// It is on
				VideoSetPixel(display, x + j, y + i, fg);
			} else {
				// It is not on
				VideoSetPixel(display, x + j, y + i, bg);
			}
		}
	}

	return 0;
}

int VideoPutGlyphs(DisplayObject* display, char* s, int x, int y, int fg, int bg, uint8_t font[][8], int wi, int hi) {
	for (int i = 0; s[i]; i++) {
		VideoPutGlyph(display, s[i], x + i * wi, y, fg, bg, font, wi, hi);
	}

	return 0;
}

int VideoFillScreen(DisplayObject* display, int color) {
	for (int y = 0; y < display->height; y++) {
		for (int x = 0; x < display->width; x++) {
			VideoSetPixel(display, x, y, color);
		}
	}

	return 0;
}

Obj* FontObCreate(char* name, uint8_t bitmap[][8], FontMapping* mapping, int width, int height, FontObject** font) {
	char* oname = (char*)MemoryAllocate(64);
	strcpy(oname, "Fonts/");
	strcpy(oname + strlen("Fonts/"), name);
	Obj* o = ObCreate(oname, "Font", sizeof(FontObject), (void**)font);
	(*font)->bitmap = (uint8_t**)bitmap;
	(*font)->mapping = (FontMapping*)mapping;
	(*font)->width = width;
	(*font)->height = height;
	return o;
}

DeviceObject* VideoLogObCreate(char* name, DisplayObject* display, FontObject* font, int fg, int bg, VideoLogObject** log) {
	DeviceObject* dev;
	IoCreateDevice(name, DEVICE_TYPE_PIPE, sizeof(VideoLogObject), videoLogDriver, &dev);
	*log = ObSub(dev, DeviceObject, VideoLogObject);
	(*log)->display = display;
	(*log)->cols = display->width / font->width;
	(*log)->rows = display->height / font->height;
	(*log)->fg = fg;
	(*log)->bg = bg;
	(*log)->font = font;
	return dev;
}

int VideoLogClear(VideoLogObject* log) {
	if (!log) return -1;

	VideoFillScreen(log->display, log->bg);
	log->row = 0;
	log->col = 0;

	return 0;
}

int rv;

int VideoLogPrintChar(VideoLogObject* log, utf8_int32_t c) {
	_asm {
		mov eax, [esp]
		mov rv, eax
	}
	if (!log) return -1;

	if (c == '\n') {
		log->col = 0;
		log->row++;
		return 0;
	}

	if (c == '\f') {
		VideoLogClear(log);
		return 0;
	}
	
	int pc = 0xFFFD;
	for (int i = 0; log->font->mapping[i].codepoint != 0xDEADBEEF; i++) {
		if (log->font->mapping[i].codepoint == c) {
			pc = log->font->mapping[i].glyph;
			break;
		}
	}

	VideoPutGlyph(log->display, pc, log->col * log->font->width, log->row * log->font->height, log->fg, log->bg, (uint8_t(*)[8])log->font->bitmap, log->font->width, log->font->height);
	log->col++;

	if (log->col > log->cols) {
		log->col = 0;
		log->row++;
	}

	if (log->row > log->rows) {
		VideoLogClear(log);
	}

	return 0;
}

int VideoLogCharUtf8(VideoLogObject* log, char* s) {
	utf8_int32_t c;
	utf8codepoint(s, &c);
	VideoLogPrintChar(log, c);
	return 0;
}

int VideoLogString(VideoLogObject* log, char* s) {
	while (*s) {
		VideoLogCharUtf8(log, s);
		s++;
	}
	return 0;
}

IoStatus VideoDriverUpdate(IoStack* req) {
	// TODO: Something with the framebuffer
	return IOSTATUS_SUCCESS;
}

void VideoInit() {
	Log("setting video mode");
	SetVideo();
	Log("set video mode");
	VbeModeInfo* vmi = (VbeModeInfo*)VBEMODEINFO_PTR;
	Log("framebuffer=%80X", vmi->framebuffer);
	DriverObCreate("Vga", "VGA Driver", &vgaDriver);
	DriverObRegisterFunction(vgaDriver, DRIVER_FUNCTION_VI_UPDATE, &VideoDriverUpdate);
	DisplayObCreate("Vga", vmi->width, vmi->height, vmi->bpp, vmi->framebuffer, vgaDriver, &mainDisplay);
	FontObCreate("Internal", internalFontBitmap, internalFontCharmap, INTERNAL_FONT_WIDTH, INTERNAL_FONT_HEIGHT, &internalFont);
	DriverObCreate("VideoLog", "Video Log Driver", &videoLogDriver);
	VideoLogObCreate("VgaLog", mainDisplay, internalFont, WHITE, NAVYBLUE, &videoLog);
	VideoFillScreen(mainDisplay, BLACK);
	VideoLogString(videoLog, "The quick brown fox jumps over the lazy dog");
	useVideoLog = true;
	useTextMode = false;
	VideoLogClear(videoLog);
	Log("Video");
}