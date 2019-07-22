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

struct Rectangle {
	int x;
	int y;
	int height;
	int width;
};

struct Ellipse {
	int x;
	int y;
	int height;
	int width;
};

struct Line {
	int x1;
	int y1;
	int x2;
	int y2;
};

struct Brush {
	Color color;
	BrushStyle style;
	Hatch hatch;
};