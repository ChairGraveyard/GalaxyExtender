#pragma once

#include "Object.h"

class CommandParserHistory : public BaseHookedObject {
	int index;
	void* history;
	uint32_t var3;
public:
	CommandParserHistory() {
		runMethod<0xA87590, void>();
	}

	~CommandParserHistory() {
		runMethod<0xA87630, void>();
	}

	int getIndex() {
		return index;
	}

	void* getHistory() {
		return history;
	}
};