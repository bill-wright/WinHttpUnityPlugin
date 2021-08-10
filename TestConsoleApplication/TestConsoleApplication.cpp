// TestConsoleApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
using namespace std;

int main()
{
	SetConsoleOutputCP(65001);
	wstring server = L"127.0.0.1";
//	wstring server = L"10.66.40.100";
	int port = 80;
	wstring method = L"GET";
	wstring command = L"WorkDefinition/WorkOrder/foo";
	size_t len = 0;
	WebResults wr = GetContent(server.c_str(), port, command.c_str());
	int http_code = wr.httpResultCode;
	len = wr.contentOutSize;
	wr.contentOut[len] = 0;
	cout << "HTTP CODE: " << http_code << " GetResult is:\n";
	printf("%s", wr.contentOut);
	cout << "\nContent size:\n";
	cout << len;
	delete[] wr.contentOut;

#if 1
	cout << "Post request:\n";
	len = 1024 * 1024;
	method = L"GET";
	command = L"DesignProductStructure/DesignComponent/IVF/GetByUuid";
	LPCSTR content = "<somexml/>";
	wr = PostContent(server.c_str(), port, command.c_str(), (unsigned char *)content, strlen(content) * sizeof(char));
	http_code = wr.httpResultCode;
	len = wr.contentOutSize;
	wr.contentOut[len] = 0;
	cout << "HTTP CODE: " << http_code << " GetResult is:\n";
	printf("%s", wr.contentOut);
	cout << "\nContent size:\n";
	cout << len;
	delete[] wr.contentOut;
#endif
}
