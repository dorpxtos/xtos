#pragma once
#include <stdint.h>

typedef uint32_t Color;

typedef enum {
	SOLID,
	HOLLOW,
	HATCHED,
} BrushStyle;

typedef enum {
	NONE,
	HORIZONTALLY,
	VERTICALLY,
	HORIZVERT,
	CHECKERBOARD
} Hatch;

typedef struct Rectangle {
	int x;
	int y;
	int height;
	int width;
} Rectangle;

typedef struct Ellipse {
	int x;
	int y;
	int height;
	int width;
} Ellipse;

typedef struct Line {
	int x1;
	int y1;
	int x2;
	int y2;
} Line;

typedef struct Brush {
	Color color;
	BrushStyle style;
	Hatch hatch;
} Brush;

typedef struct WindowClass {
	int titlebarBackground;
	int titlebarForeground;
	char* className;
} WindowClass;

typedef struct WindowConfig {
	char* title;
	int x;
	int y;
	int width;
	int height;
} WindowConfig;