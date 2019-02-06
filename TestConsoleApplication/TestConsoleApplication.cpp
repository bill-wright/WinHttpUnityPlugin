// TestConsoleApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
using namespace std;

int main()
{
	SetConsoleOutputCP(65001);
	wstring server = L"172.16.111.144";
	int port = 80;
	wstring method = L"GET";
	wstring command = L"cubetser/api/Dashboard/58";
	LPCSTR content = static_cast<char*>(GetContent(server.c_str(), port, command.c_str()));
	auto resultLenght = std::strlen(content);
	cout << "GetResult is:\n";
	printf("%s", content);
	cout << "\nAdress result:\n";
	wcout << *content;
	cout << "\nContent size:\n";
	wcout << resultLenght;
	delete[] content;
	Sleep(100);

	//	cout << "Post request:\n";
	//LPCSTR content = "{\"Login\":null,\"Date\":\"01/31/2019\",\"Message\":\"Another message to log\"}";
	//PostContent(server.c_str(), port, L"api/log", (unsigned char *)content, strlen(content) * sizeof(char));
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
