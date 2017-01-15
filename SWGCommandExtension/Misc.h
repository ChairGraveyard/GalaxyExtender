#pragma once

#include "Object.h"
#include "soewrappers.h"

class Misc {
public:
	static char* DuplicateString(const char* str) {
		return Call<0x00AC15E0, char*, const char*>::run(str);
	}
};