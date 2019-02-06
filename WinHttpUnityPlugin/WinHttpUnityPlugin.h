#pragma once

#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

#ifndef DllExport
#define DllExport __declspec (dllexport)
#endif

extern "C"
{
	DllExport void* GetContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod);
	DllExport void PostContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, byte *content, int contentSize);
}


