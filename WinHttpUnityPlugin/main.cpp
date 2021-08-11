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

WebResults FormatError(const char *msg, int errorMessageID) {
	WebResults wr;

	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		GetModuleHandle(TEXT("winhttp.dll")), errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::stringstream ss;
	ss << "ERROR: " << msg << " (code " << errorMessageID << ") " << messageBuffer;

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	ss.seekg(0, ss.end);
	auto length = ss.tellg();
	ss.seekg(0, ss.beg);
	wr.contentOutSize = length;
	wr.contentOut = new byte[1 + length];
	ss.read((char *)(wr.contentOut), length);
	return wr;
}


// trim from left
inline std::wstring& ltrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

// trim from right
inline std::wstring& rtrim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v")
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from left & right
inline std::wstring& trim(std::wstring& s, const wchar_t* t = L" \t\n\r\f\v")
{
	return ltrim(rtrim(s, t), t);
}


WebResults ExecuteRequest(LPCWSTR server, INTERNET_PORT port, LPCWSTR httpMethod, LPCWSTR apiMethod, LPCWSTR in_headers, byte* content, int contentSize)
{
	printf("Input parameters:\n");
	printf("server: %ls\n", server);
	printf("port: %hu\n", port);
	printf("httpMethod: %ls\n", httpMethod);
	printf("apiMethod: %ls\n", apiMethod);
	printf("headers: %ls\n", in_headers);
	if (content)
	{
		printf("content is presented with size of %u bytes\n", (unsigned)contentSize);
	}

	DWORD dwStatusCode = -1;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	BOOL bResults = FALSE;
	HINTERNET hConnect = nullptr, hRequest = nullptr;
	std::string result = "";
	LPVOID lpOutBuffer = NULL;
	DWORD plLogon = WINHTTP_AUTOLOGON_SECURITY_LEVEL_LOW;
	std::wstring headers = in_headers;
	WebResults wr;


	// Use WinHttpOpen to obtain a session handle.
	const HINTERNET hSession = WinHttpOpen(L"WinHTTP/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession)	{
		wr = FormatError("WinHttpOpen Failed", GetLastError());
		goto all_done;
	}

	// Specify an HTTP server.
	hConnect = WinHttpConnect(hSession, server, port, 0);
	if (!hConnect) {
		wr = FormatError("WinHttpConnect Failed", GetLastError());
		goto all_done;
	}

	// Create an HTTP request handle.
	hRequest = WinHttpOpenRequest(hConnect, httpMethod, apiMethod,
			nullptr, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_REFRESH);
	if (!hRequest) {
		wr = FormatError("WinHttpOpenRequest Failed", GetLastError());
		goto all_done;
	}

	// Send a request.
	WinHttpSetOption(hRequest, WINHTTP_OPTION_AUTOLOGON_POLICY, reinterpret_cast<LPVOID*>(&plLogon), sizeof plLogon);

	headers = trim(headers);

	bResults = WinHttpSendRequest(hRequest, headers.c_str(), -1L, content, (DWORD)contentSize, (DWORD)contentSize, 0);
	if (!bResults) {
		wr = FormatError("WinHttpSendRequest Failed", GetLastError());
		goto all_done;
	}

	// End the request.
	bResults = WinHttpReceiveResponse(hRequest, nullptr);
	if (!bResults) {
		wr = FormatError("WinHttpReceiveResponse Failed", GetLastError());
		goto all_done;
	}

	// Check for HTTP errors
	dwSize = sizeof(dwStatusCode);
	WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode,
		&dwSize, WINHTTP_NO_HEADER_INDEX);

	// if error, return the headers in the contents
	wr.httpResultCode = dwStatusCode;
	if (dwStatusCode < 200 || dwStatusCode > 299) {
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_TEXT,
			WINHTTP_HEADER_NAME_BY_INDEX, NULL,
			&dwSize, WINHTTP_NO_HEADER_INDEX);

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			char* buf = new char[dwSize];
			if (!buf) {
				printf("out of memory");
				goto all_done;
			}
			// Now, use WinHttpQueryHeaders to retrieve the header.
			bResults = WinHttpQueryHeaders(hRequest,
				WINHTTP_QUERY_STATUS_TEXT,
				WINHTTP_HEADER_NAME_BY_INDEX,
				buf, &dwSize,
				WINHTTP_NO_HEADER_INDEX);

			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
			std::string ws = converter.to_bytes((wchar_t *)buf);
			wr.contentOut = (byte *)GlobalAlloc(GMEM_FIXED, ws.length());
			memcpy(wr.contentOut, ws.c_str(), ws.length());
			wr.contentOutSize = ws.length();
		}
		goto all_done;
	}


	// Keep checking for data until there is nothing left.
	{
		std::stringstream ss;
		unsigned int idx = 0;
		do
		{
			// Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				wr = FormatError("WinHttpQueryDataAvailable Failed", GetLastError());
				break;
			}

			{
				auto buf = new uint8_t[dwSize];
				if (!buf) {
					printf("Out of memory error");
					break;
				}
				if (!WinHttpReadData(hRequest, buf, dwSize, &dwDownloaded)) {
					wr = FormatError("WinHttpQueryDataAvailable Failed", GetLastError());
					break;
				}
				ss.write((char const *)buf, dwDownloaded);
				delete[]buf;
			}
		} while (dwSize > 0);

		ss.seekg(0, ss.end);
		auto length = ss.tellg();
		ss.seekg(0, ss.beg);
		wr.contentOut = (byte *)GlobalAlloc(GMEM_FIXED, length);
		if (!wr.contentOut) {
			printf("Out of memory for content %d bytes", length);
		}
		ss.read((char *)(wr.contentOut), length);
		wr.contentOutSize = length;
	}

all_done:
	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	// printf("\nResult in one string:\n");
	// printf("%s\n", result.c_str());

	return wr;
}

DllExport WebResults PostContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, LPCWSTR headers, byte *content, size_t contentSize)
{
	return ExecuteRequest(server, port, L"POST", apiMethod, headers, content, contentSize);
}

DllExport WebResults GetContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, LPCWSTR headers)
{
	return ExecuteRequest(server, port, L"GET", apiMethod, headers, nullptr, 0);
}
