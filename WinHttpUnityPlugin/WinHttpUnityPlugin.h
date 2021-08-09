#pragma once

#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

#ifndef DllExport
#define DllExport __declspec (dllexport)
#endif

extern "C"
{
	DllExport int GetContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, byte *contentOut, size_t *contentOutSize);
	DllExport int PostContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, byte *content, size_t contentSize, byte *contentOut, size_t *contentOutSize);
}


