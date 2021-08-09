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
	size_t len = 1024 * 1024;
	byte* content_out = new byte[len];
	int http_code = GetContent(server.c_str(), port, command.c_str(), content_out, &len);
	content_out[len] = 0;
	cout << "HTTP CODE: " << http_code << " GetResult is:\n";
	printf("%s", content_out);
	cout << "\nContent size:\n";
	wcout << len;

	cout << "Post request:\n";
	len = 1024 * 1024;
	method = L"GET";
	command = L"DesignProductStructure/DesignComponent/IVF/GetByUuid";
	LPCSTR content = "<somexml/>";
	PostContent(server.c_str(), port, command.c_str(), (unsigned char *)content, strlen(content) * sizeof(char), content_out, &len);
	content_out[len] = 0;
	cout << "POST HTTP CODE: " << http_code << " GetResult is:\n";
	printf("%s\n", content_out);
	cout << "Content size:\n";
	wcout << len;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
