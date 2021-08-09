#include "WinHttpUnityPlugin.h"
#include <cstdio>
#include <string>
#include <sstream>
#include <codecvt>

#ifndef DllExport
#define DllExport __declspec (dllexport)
#endif


//std::wstring_convert<std::codecvt<char16_t, char, std::mbstate_t>, char16_t> convert;

BSTR UTF8toBSTR(char* input)
{
	BSTR result = nullptr;
	const int lenA = lstrlenA(input);
	const int lenW = ::MultiByteToWideChar(CP_UTF8, 0, input, lenA, nullptr, 0);
	if (lenW > 0)
	{
		result = ::SysAllocStringLen(0, lenW);
		::MultiByteToWideChar(CP_UTF8, 0, input, lenA, result, lenW);
	}
	return result;
}

int FormatError(byte *contentOut, size_t *contentOutSize, const char *msg, int errorMessageID) {
	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		GetModuleHandle(TEXT("winhttp.dll")), errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	*contentOutSize = snprintf((char *)contentOut, *contentOutSize, "ERROR: %s (code %d) %s", msg, errorMessageID, messageBuffer);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);
	return *contentOutSize;
}


int ExecuteRequest(LPCWSTR server, INTERNET_PORT port, LPCWSTR httpMethod, LPCWSTR apiMethod, byte* content,
                           size_t contentSize, byte *contentOut, size_t *contentOutSize)
{
	printf("Input parameters:\n");
	printf("server: %ls\n", server);
	printf("port: %hu\n", port);
	printf("httpMethod: %ls\n", httpMethod);
	printf("apiMethod: %ls\n", apiMethod);
	if (content)
	{
		printf("content is presented with size of %d bytes", contentSize);
	}

	DWORD dwStatusCode = -1;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	BOOL bResults = FALSE;
	HINTERNET hConnect = nullptr, hRequest = nullptr;
	std::string result = "";
	LPVOID lpOutBuffer = NULL;
	DWORD plLogon = WINHTTP_AUTOLOGON_SECURITY_LEVEL_LOW;
	std::wstring headers = L"";


	// Use WinHttpOpen to obtain a session handle.
	const HINTERNET hSession = WinHttpOpen(L"WinHTTP/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession)	{
		FormatError(contentOut, contentOutSize, "WinHttpOpen Failed", GetLastError());
		goto all_done;
	}

	// Specify an HTTP server.
	hConnect = WinHttpConnect(hSession, server, port, 0);
	if (!hConnect) {
		FormatError(contentOut, contentOutSize, "WinHttpConnect Failed", GetLastError());
		goto all_done;
	}

	// Create an HTTP request handle.
	hRequest = WinHttpOpenRequest(hConnect, httpMethod, apiMethod,
			nullptr, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_REFRESH);
	if (!hRequest) {
		FormatError(contentOut, contentOutSize, "WinHttpOpenRequest Failed", GetLastError());
		goto all_done;
	}

	// Send a request.
	WinHttpSetOption(hRequest, WINHTTP_OPTION_AUTOLOGON_POLICY, reinterpret_cast<LPVOID*>(&plLogon), sizeof plLogon);

	if (content)
	{
		headers += L"Content-Type: application/xml\nContent-Length: ";
		headers += std::to_wstring(contentSize);
	}

	bResults = WinHttpSendRequest(hRequest, headers.c_str(), -1L, content, contentSize,	contentSize, 0);
	if (!bResults) {
		FormatError(contentOut, contentOutSize, "WinHttpSendRequest Failed", GetLastError());
		goto all_done;
	}

	// End the request.
	bResults = WinHttpReceiveResponse(hRequest, nullptr);
	if (!bResults) {
		FormatError(contentOut, contentOutSize, "WinHttpReceiveResponse Failed", GetLastError());
		goto all_done;
	}

	// Check for HTTP errors
	dwSize = sizeof(dwStatusCode);
	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode,
		&dwSize, WINHTTP_NO_HEADER_INDEX);

	// if error, return the headers in the contents

	if (dwStatusCode < 200 || dwStatusCode > 299) {
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_TEXT,
			WINHTTP_HEADER_NAME_BY_INDEX, NULL,
			&dwSize, WINHTTP_NO_HEADER_INDEX);

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			if (dwSize <= *contentOutSize) {
				// Now, use WinHttpQueryHeaders to retrieve the header.
				bResults = WinHttpQueryHeaders(hRequest,
					WINHTTP_QUERY_STATUS_TEXT,
					WINHTTP_HEADER_NAME_BY_INDEX,
					contentOut, &dwSize,
					WINHTTP_NO_HEADER_INDEX);

				std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
				std::string ws = converter.to_bytes((wchar_t *)contentOut);
				memcpy(contentOut, ws.c_str(), ws.length());
				*contentOutSize = ws.length();
			}
			else {
				unsigned int availableBytes = *contentOutSize;
				*contentOutSize = snprintf((char *)contentOut, availableBytes-1, "ERROR: Need %d bytes to show error.  Only have %d bytes", dwSize, availableBytes);
			}
		}
		goto all_done;
	}


	// Keep checking for data until there is nothing left.
	{
		unsigned int idx = 0;
		do
		{
			// Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				FormatError(contentOut, contentOutSize, "WinHttpQueryDataAvailable Failed", GetLastError());
				break;
			}

			if ((idx + dwSize) > *contentOutSize) {
				*contentOutSize = snprintf((char *)contentOut, *contentOutSize, "Size is bigger than max: %u", *contentOutSize);

				break;
			}

			if (!WinHttpReadData(hRequest, static_cast<LPVOID>(contentOut + idx), dwSize, &dwDownloaded)) {
				FormatError(contentOut, contentOutSize, "WinHttpQueryDataAvailable Failed", GetLastError());
				break;
			}
			idx += dwDownloaded;
		} while (dwSize > 0);
		*contentOutSize = idx;
	}

all_done:
	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	// printf("\nResult in one string:\n");
	// printf("%s\n", result.c_str());

	return dwStatusCode;
}

DllExport int PostContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, byte *content, size_t contentSize, byte *contentOut, size_t *contentOutSize)
{
	return ExecuteRequest(server, port, L"POST", apiMethod, content, contentSize, contentOut, contentOutSize);
}

DllExport int GetContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, byte *contentOut, size_t *contentOutSize)
{
	return ExecuteRequest(server, port, L"GET", apiMethod, nullptr, 0, contentOut, contentOutSize);

}
