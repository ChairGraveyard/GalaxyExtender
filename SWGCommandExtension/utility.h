#pragma once

#include <windows.h>
#include <string>
#include <vector>

namespace std 
{
	void split(const string& s, char delim, vector<string>& v);
	void split(const wstring& s, wchar_t delim, vector<wstring>& v);

	wstring s2ws(const string& str);
	string ws2s(const wstring& wstr);
}

void writeJmp(BYTE* address, DWORD jumpTo, DWORD length);

// We redirect to client using push/ret trick cause masm cant jmp to absolute values 
#define JUMPTOCLIENT(addyval) __asm	\
	{							\
		__asm push addyval		\
		__asm ret				\
	}

