#include "WinHttpUnityPlugin.h"
#include <cstdio>
#include <string>

#ifndef DllExport
#define DllExport __declspec (dllexport)
#endif

BSTR ANSItoBSTR(char* input)
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

const char* ExecuteRequest(LPCWSTR server, INTERNET_PORT port, LPCWSTR httpMethod, LPCWSTR apiMethod, byte* content,
                           int contentSize)
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

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL bResults = FALSE;
	HINTERNET hConnect = nullptr, hRequest = nullptr;
	std::string result = "";

	// Use WinHttpOpen to obtain a session handle.
	const HINTERNET hSession = WinHttpOpen(L"WinHTTP/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
	{
		hConnect = WinHttpConnect(hSession, server,
			port, 0);
	}

	// Create an HTTP request handle.
	if (hConnect)
	{
		hRequest = WinHttpOpenRequest(hConnect, httpMethod, apiMethod,
			nullptr, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_REFRESH);
	}

	// Send a request.
	if (hRequest)
	{
		DWORD plLogon = WINHTTP_AUTOLOGON_SECURITY_LEVEL_LOW;
		WinHttpSetOption(hRequest, WINHTTP_OPTION_AUTOLOGON_POLICY, reinterpret_cast<LPVOID*>(&plLogon), sizeof plLogon);

		std::wstring headers = L"";

		if (content)
		{
			headers += L"Content-Type: application/json\nContent-Lenght: ";
			headers += std::to_wstring(contentSize);
		}

		bResults = WinHttpSendRequest(hRequest, headers.c_str(), -1L, content, contentSize,	contentSize, 0);
	}

	// End the request.
	if (bResults)
	{
		bResults = WinHttpReceiveResponse(hRequest, nullptr);
	}

	// Keep checking for data until there is nothing left.
	if (bResults)
	{
		do
		{
			// Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n",
					GetLastError());
			}

			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				dwSize = 0;
			}
			else
			{
				// Read the data.
				ZeroMemory(pszOutBuffer, dwSize + 1);

				if (!WinHttpReadData(hRequest, static_cast<LPVOID>(pszOutBuffer),
					dwSize, &dwDownloaded))
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				else
				{
					// printf("%s", pszOutBuffer);
					result += pszOutBuffer;
				}

				// Free the memory allocated to the buffer.
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	// Report any errors.
	if (!bResults)
	{
		printf("Error %d has occurred.\n", GetLastError());
	}

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	// printf("\nResult in one string:\n");
	// printf("%s\n", result.c_str());

	const auto length = result.length();
	//return ANSItoBSTR(const_cast<char*>(result.c_str()));	
	char* resArray = new char[length + 1];
	strcpy_s(resArray, length + 1, result.c_str());
	resArray[length + 1] = '\0';
	return resArray;
}

DllExport void PostContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod, byte *content, int contentSize)
{
	ExecuteRequest(server, port, L"POST", apiMethod, content, contentSize);
}

DllExport const char* GetContent(LPCWSTR server, INTERNET_PORT port, LPCWSTR apiMethod)
{
	return ExecuteRequest(server, port, L"GET", apiMethod, nullptr, 0);
}
