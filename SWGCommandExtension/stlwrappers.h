#pragma once

#include <string>

#define STRALLOCATOR_ADDRESS 0x012EA770
#define STRDEALLOCATOR2_ADDRESS 0x012EA920

class stlportstring {
	char* start;
	char* finish;
	char* endOfStorage;

protected:
	typedef void*(__cdecl* strallocator_t)(int);
	typedef void(__cdecl* strdeallocator1_t)(int);
	typedef void(__cdecl* strdeallocator2_t)(void*, int);

	void alloc(int size);
	void dealloc();
public:
	stlportstring() {
		alloc(8);
	}

	stlportstring(const stlportstring& s) {
		alloc(s.size() + 1);

		copyFrom(s.begin(), s.size());
	}

	stlportstring(const char* cstring) {
		int len = strlen(cstring);

		alloc(len + 1);

		copyFrom(cstring, len);
	}

	stlportstring& operator=(const stlportstring& s) {
		dealloc();

		int len = s.size();

		alloc(len + 1);

		copyFrom(s.start, len);
	}

	~stlportstring() {
		dealloc();
	}

	void copyFrom(const char* str, int len) {
		memcpy(start, str, len);
		finish = start + len;

		*(finish + 1) = 0;
	}

	const char* c_str() const {
		return start;
	}

	const char* begin() const {
		return start;
	}

	const char* end() const {
		return finish;
	}

	char* begin() {
		return start;
	}

	char* end() {
		return finish;
	}

	char operator[](uint32_t i) const {
		return start[i];
	}

	char& operator[](uint32_t i) {
		return start[i];
	}

	int size() const {
		return finish - start;
	}

	uint32_t hashCode() const;

	static uint32_t hashCode(const char* string, uint32_t startCRC = 0xFFFFFFFF);
};