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
//	wstring command = L"WorkDefinition/WorkOrder/foo";
	wstring command = L"dummy/WorkArea/8ed2fce4-fc11-2db0-e053-24690b99b275";
	size_t len = 0;
	WebResults wr = GetContent(server.c_str(), port, command.c_str());
	int http_code = wr.httpResultCode;
	len = wr.contentOutSize;
	cout << "HTTP CODE: " << http_code << " GetResult is:\n";
	std::string s(reinterpret_cast<const char *>(wr.contentOut), len);
	cout << s.c_str();
	cout << "\nContent size:\n";
	cout << len << endl;
	GlobalFree(wr.contentOut);


#if 1
	cout << "Post request:\n";
	len = 1024 * 1024;
	method = L"GET";
	command = L"DesignProductStructure/DesignComponent/IVF/GetByUuid";
	LPCSTR content = "<somexml/>";
	wr = PostContent(server.c_str(), port, command.c_str(), (unsigned char *)content, strlen(content) * sizeof(char));
	http_code = wr.httpResultCode;
	len = wr.contentOutSize;
	cout << "HTTP CODE: " << http_code << " GetResult is:\n";
	std::string s1(reinterpret_cast<const char *>(wr.contentOut), len);
	cout << s1.c_str();
	cout << "\nContent size:\n";
	cout << len << endl;
	GlobalFree(wr.contentOut);
#endif
}
