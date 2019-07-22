#pragma once
#include <DeclSpec.h>
#include <Handle.h>
#include <Graphics.h>

DllExport Handle GetDC(Handle);
DllExport void GdiSetPixel(Handle, int, int, int);
DllExport void GdiDrawRect(Handle, Rectangle*, Handle);
DllExport void GdiDrawRectOutline(Handle, int, int, int, int, int);
DllExport void GdiDrawLine(Handle, int, int, int, int, int);
DllExport void GdiDrawEllipse(Handle, int, int, int, int, int);
DllExport Handle GdiCreateSolidBrush(Color);
DllExport Handle GdiCreateHatchedBrush(Color, Hatch);
DllExport void GdiFlush(Handle);
DllExport int GdiKrnlInit();
