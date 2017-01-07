#include "stdafx.h"

#include "utility.h"

#include <locale>
#include <codecvt>

/// General Utilities
///
namespace std 
{
	// Split string to vector.
	void split(const string& s, char delim, vector<string>& v) 
	{
		auto i = 0;
		auto pos = s.find(delim);
		while (pos != string::npos) 
		{
			v.push_back(s.substr(i, pos - i));
			i = ++pos;
			pos = s.find(delim, pos);

			if (pos == string::npos)
				v.push_back(s.substr(i, s.length()));
		}
	}

	// Split wide-string to vector.
	void split(const wstring& s, wchar_t delim, vector<wstring>& v) 
	{
		auto i = 0;
		auto pos = s.find(delim);
		while (pos != wstring::npos) 
		{
			v.push_back(s.substr(i, pos - i));
			i = ++pos;
			pos = s.find(delim, pos);

			if (pos == wstring::npos)
				v.push_back(s.substr(i, s.length()));
		}
	}

	// String to wide-string.
	wstring s2ws(const string& str)
	{
		using convert_typeX = codecvt_utf8<wchar_t>;
		wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	// Wide-string to string.
	string ws2s(const wstring& wstr)
	{
		using convert_typeX = codecvt_utf8<wchar_t>;
		wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
}

///
///

/// Memory Utilties
///
void writeJmp(BYTE* address, DWORD jumpTo, DWORD length)
{
	DWORD oldProtect, newProtect, relativeAddress;

	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

	relativeAddress = (DWORD)(jumpTo - (DWORD)address) - 5;
	*address = 0xE9;
	*((DWORD *)(address + 0x1)) = relativeAddress;

	for (DWORD x = 0x5; x < length; x++)
	{
		*(address + x) = 0x90;
	}

	VirtualProtect(address, length, oldProtect, &newProtect);
}
///
///
