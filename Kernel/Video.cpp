#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <Colors.h>
#include <Log.h>
#include <Paging.h>
#include <Io.h>
#include <ObjectManager.h>
#include <MemoryAllocator.h>
#include <Device.h>
#include <Driver.h>
#include <TextMode.h>
#include <Video.h>

#include "InternalFont.h"

extern "C" extern void SetVideo();

DriverObject* vgaDriver;
DriverObject* videoLogDriver;
DisplayObject* mainDisplay;
FontObject* internalFont;
VideoLogObject* videoLog;

bool useVideoLog = false;

static int pow2(int x) {
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
	for (int i = 0; i < (size + (size / 2)) / PAGE_SIZE; i++) {
		MapPage(kernelPagemap, f + i * PAGE_SIZE, f + i * PAGE_SIZE, 0x3);
	}

	return dev;
}

int VideoPutPixel(DisplayObject* display, int x, int y, int color) {
	if (!display) return -1;
	
	if (display->bpp == 24) {
		int p = (y * display->width + x) * (display->bpp / 8);
		display->framebuffer[p] = color & 0xFF;
		display->framebuffer[p+1] = (color >> 8) & 0xFF;
		display->framebuffer[p+2] = (color >> 16) & 0xFF;
	}

	return 0;
}

int VideoPutChar(DisplayObject* display, char c, int x, int y, int fg, int bg, uint8_t* font, int wi, int hi) {
	// For every row in the font height
	for (char i = 0; i < hi; i++) {
		// Let fl = the bitmap of the character that is going to be written
		unsigned short fl = font[(c*hi + i)*(wi / 8)];
		// For every column in the font width
		for (char j = 0; j < wi; j++) {
			// Let fc = The pixel of the bitmap that is going to be written
			unsigned short fc = (fl >> (wi - j)) & 1;

			// Check if the pixel is on or not
			if (fc == 1) {
				// It is on
				VideoPutPixel(display, x + j, y + i, fg);
			} else {
				// It is not on
				VideoPutPixel(display, x + j, y + i, bg);
			}
		}
	}

	return 0;
}

int VideoPutString(DisplayObject* display, char* s, int x, int y, int fg, int bg, uint8_t* font, int wi, int hi) {
	for (int i = 0; s[i]; i++) {
		VideoPutChar(display, s[i], x + i * wi, y, fg, bg, font, wi, hi);
	}

	return 0;
}

int VideoFillScreen(DisplayObject* display, int color) {
	for (int y = 0; y < display->height; y++) {
		for (int x = 0; x < display->width; x++) {
			VideoPutPixel(display, x, y, color);
		}
	}

	return 0;
}

Obj* FontObCreate(char* name, uint8_t* bitmap, int width, int height, FontObject** font) {
	char* oname = (char*)MemoryAllocate(64);
	strcpy(oname, "Fonts/");
	strcpy(oname + strlen("Fonts/"), name);
	Obj* o = ObCreate(oname, "Font", sizeof(FontObject), (void**)font);
	(*font)->bitmap = bitmap;
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

int VideoLogPrintChar(VideoLogObject* log, char c) {
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

	VideoPutChar(log->display, c, log->col * log->font->width, log->row * log->font->height, log->fg, log->bg, log->font->bitmap, log->font->width, log->font->height);
	log->col++;

	if (log->col > log->cols) {
		log->col = 0;
		log->row++;
	}

	return 0;
}

int VideoLogPrintString(VideoLogObject* log, char* s) {
	while (*s) VideoLogPrintChar(log, *s++);
	return 0;
}

void VideoInit() {
	VbeModeInfo* vmi = (VbeModeInfo*)0x9000;
	LogPrint("fb=%x", &vmi->framebuffer);
	DriverObCreate("Vga", "VGA Driver", &vgaDriver);
	SetVideo();
	DisplayObCreate("Vga", vmi->width, vmi->height, vmi->bpp, vmi->framebuffer, vgaDriver, &mainDisplay);
	FontObCreate("Internal", internalFontBitmap, INTERNAL_FONT_WIDTH, INTERNAL_FONT_HEIGHT, &internalFont);
	DriverObCreate("VideoLog", "Video Log Driver", &videoLogDriver);
	VideoLogObCreate("VgaLog", mainDisplay, internalFont, WHITE, BLACK, &videoLog);
	VideoFillScreen(mainDisplay, BLACK);
	VideoLogPrintString(videoLog, "The quick brown fox jumps over the lazy dog");
	useVideoLog = true;
	useTextMode = false;
	LogPrint("Video");
}