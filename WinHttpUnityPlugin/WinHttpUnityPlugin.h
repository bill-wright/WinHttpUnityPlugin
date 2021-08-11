#pragma once

#include <Windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

#ifndef DllExport
#define DllExport __declspec (dllexport)
#endif

typedef struct WebResults {
	int httpResultCode;
	unsigned int contentOutSize;
	byte *contentOut;
} WebResults;

extern "C"
{
	DllExport WebResults GetContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, LPCWSTR headers);
	DllExport WebResults PostContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, LPCWSTR headers, byte *content, size_t contentSize);
}


