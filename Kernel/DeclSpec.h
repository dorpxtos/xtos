#pragma once

#ifdef __cplusplus
#define DllExport extern "C" __declspec(dllexport)
#define DllImport extern "C" __declspec(dllimport)
#else
#define DllExport __declspec(dllexport)
#define DllImport __declspec(dllimport)
#endif