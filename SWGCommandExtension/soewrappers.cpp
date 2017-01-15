#include "stdafx.h"
#include "soewrappers.h"
#include "NetworkId.h"

NetworkId const NetworkId::empty;

SET_CLIENT_STATIC(soe::unicode::empty_string, 0x01918970);

soe::string::string() : soe::stringbase_t<char>() {
}

soe::string::string(uint32_t initialCapacity) : soe::stringbase_t<char>(initialCapacity) {
}

soe::string::string(const soe::string& s) : soe::stringbase_t<char>(s) {
}

soe::string::string(soe::string&& s) noexcept : soe::stringbase_t<char>(std::move(s)) {
}

soe::string::string(const char* cstring) : soe::stringbase_t<char>(cstring, strlen(cstring)) {
}

soe::string::string(const char* cstring, uint32_t length) : soe::stringbase_t<char>(cstring, length) {
}

std::size_t soe::string::find(const string& str, std::size_t pos) const noexcept {
	auto result = strstr(begin() + pos, str.begin());

	if (result) {
		return result - begin();
	} else {
		return std::string::npos;
	}
}

soe::string& soe::string::operator=(const string& s) {
	if (&s == this)
		return *this;

	stringbase_t<char>::operator=(s);

	return *this;
}

soe::string soe::string::operator+(const soe::string & rhs) const {
	uint32_t leftSize = size();
	uint32_t rightSize = rhs.size();

	soe::string newstring(leftSize + rightSize); //this allocates the extra finish character

	memcpy(newstring.start, start, leftSize);
	memcpy(newstring.start + leftSize, rhs.start, rightSize);
	newstring.finish = newstring.start + (leftSize + rightSize);
	*(newstring.finish) = 0;

	return newstring;
}

soe::string soe::string::operator+(const char* rhs) const {
	uint32_t leftSize = size();
	uint32_t rightSize = strlen(rhs);

	soe::string newstring(leftSize + rightSize); //this allocates the extra finish character

	memcpy(newstring.start, start, leftSize);
	memcpy(newstring.start + leftSize, rhs, rightSize);
	newstring.finish = newstring.start + (leftSize + rightSize );
	*(newstring.finish) = 0;

	return newstring;
}

soe::string soe::operator+(const char* s, const soe::string& s2) {
	int leftSize = strlen(s);
	int rightSize = s2.size();

	soe::string newstr(leftSize + rightSize);
	
	for (int i = 0; i < leftSize; ++i) {
		newstr.push_back(s[i]);
	}

	for (int i = 0; i < rightSize; ++i) {
		newstr.push_back(s2[i]);
	}

	return newstr;
}

soe::string & soe::string::operator+=(const soe::string& rhs) {
	uint32_t leftSize = size();
	uint32_t rightSize = rhs.size();

	ensureCapacity(leftSize + rightSize + 1);

	memcpy(start + leftSize, rhs.start, rightSize);

	finish = start + leftSize + rightSize;
	*(finish) = 0;

	return *this;
}

soe::string & soe::string::operator+=(const char* rhs) {
	uint32_t leftSize = size();
	uint32_t rightSize = strlen(rhs);

	ensureCapacity(leftSize + rightSize + 1);

	memcpy(start + leftSize, rhs, rightSize);

	finish = start + leftSize + rightSize;
	*(finish) = 0;

	return *this;
}

bool soe::string::operator==(const soe::string& str) const {
	if (this == &str)
		return true;

	return str.operator==(start);
}

bool soe::string::operator==(const char* str) const {
	return strcmp(str, start) == 0;
}

bool soe::operator==(const char* s, const soe::string& s2) {
	return s2.operator==(s);
}

/*std::size_t soe::string::hash() const {
	return hashCode();
}*/

uint32_t soe::string::hashCode() const {
	uint32_t CRC = 0xFFFFFFFF;
	const char* string = begin();

	return hashCode(string, CRC);
}
/*
uint32_t soe::string::hashCode(const char * string, uint32_t startCRC) {
	uint32_t CRC = startCRC;

	for (; *string; ++string) {
		CRC = soecrctable[((CRC >> 24) ^ static_cast<uint8_t>(*string)) & 0xFF] ^ (CRC << 8);
	}

	return ~CRC;
}
*/
soe::unicode::unicode() : soe::stringbase_t<wchar_t>() {
}

soe::unicode::unicode(soe::unicode&& s) noexcept : soe::stringbase_t<wchar_t>(std::move(s)) {
}

soe::unicode::unicode(const uint32_t initialCapacity) : soe::stringbase_t<wchar_t>(initialCapacity) {
}

soe::unicode::unicode(const soe::unicode& s) : soe::stringbase_t<wchar_t>(s) {
}

soe::unicode::unicode(const wchar_t* cstring) : soe::stringbase_t<wchar_t>(cstring, wcslen(cstring)) {
}

soe::unicode::unicode(const wchar_t* cstring, uint32_t length) : soe::stringbase_t<wchar_t>(cstring, length) {
}

soe::unicode::unicode(const char* cstring) : soe::stringbase_t<wchar_t>(strlen(cstring)) {
	fromAscii(cstring);
}

soe::unicode::unicode(const char* cstring, uint32_t length) : soe::stringbase_t<wchar_t>(length) {
	fromAscii(cstring);
}

void soe::unicode::fromAscii(const char* cstring) {
	uint32_t i = 0;

	for (; cstring[i]; ++i) {
		start[i] = cstring[i];
	}

	finish = start + i;
	*(finish) = 0;
}

soe::string soe::unicode::toAscii() const {
	const auto thisSize = size();

	soe::string ascii(thisSize);

	for (uint32_t i = 0; i < thisSize; ++i) {
		ascii.push_back(static_cast<char>(start[i]));
	}

	return ascii;
}

std::size_t soe::unicode::find(const unicode& str, std::size_t pos) const noexcept {
	const auto result = wcsstr(begin() + pos, str.begin());

	if (result) {
		return result - begin();
	} else {
		return std::wstring::npos;
	}
}

soe::unicode& soe::unicode::operator=(const soe::unicode& s) {
	if (&s == this)
		return *this;

	soe::stringbase_t<wchar_t>::operator=(s);

	return *this;
}

bool soe::unicode::operator==(const soe::unicode& str) const {
	if (this == &str)
		return true;

	int thisLength = size() - 1;
	const auto* strStart = str.begin();

	int i = 0;
	for (; strStart[i] && start[i]; ++i) {
		if (start[i] != strStart[i])
			return false;
	}

	return thisLength == i;
}

bool soe::unicode::operator==(const wchar_t* str) const {
	return wcscmp(begin(), str) == 0;
}

bool soe::unicode::operator==(const char* str) const {
	int thisLength = size() - 1;

	int i = 0;
	for (; str[i] && start[i]; ++i) {
		if (start[i] != str[i])
			return false;
	}

	return thisLength == i;
}

bool soe::operator==(const char* s, const soe::unicode& s2) {
	return s2.operator==(s);
}

soe::unicode soe::unicode::operator+(const soe::unicode & rhs) const {
	uint32_t leftSize = size();
	uint32_t rightSize = rhs.size();

	soe::unicode newstring(leftSize + rightSize); //this allocates the extra finish character

	memcpy(newstring.start, start, leftSize * sizeof(wchar_t));
	memcpy(newstring.start + leftSize, rhs.start, rightSize * sizeof(wchar_t));
	newstring.finish = newstring.start + (leftSize + rightSize);
	*(newstring.finish) = 0;

	return newstring;
}

soe::unicode& soe::unicode::operator+=(const unicode& rhs) {
	if (this == &rhs)
		return *this;

	auto leftSize = size();
	auto rightSize = rhs.size();

	ensureCapacity(leftSize + rightSize + 1);

	memcpy(start + leftSize, rhs.start, rightSize * sizeof(wchar_t));

	finish = start + leftSize + rightSize;
	*(finish) = 0;

	return *this;
}

soe::unicode& soe::unicode::operator+=(const wchar_t* rhs) {
	auto leftSize = size();
	auto rightSize = wcslen(rhs);

	ensureCapacity(leftSize + rightSize + 1);

	memcpy(start + leftSize, rhs, rightSize * sizeof(wchar_t));

	finish = start + leftSize + rightSize;
	*(finish) = 0;

	return *this;
}