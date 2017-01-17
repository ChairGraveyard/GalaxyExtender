#pragma once

#include "Object.h"

class UIMessage : public BaseHookedObject {
public:
	int type;
	bool leftShift;
	bool rightShift;
	bool leftControl;
	bool rightControl;
	bool leftAlt;
	bool rightAlt;

	bool leftMouseDown;
	bool middleMouseDown;
	bool rightMouseDown;
	uint16_t keystroke;
	uint16_t data;
	long mouseX;
	long mouseY;

	UIWidget* source = nullptr;
	UIWidget* object = nullptr;
	UIWidget* target = nullptr;
public:

	uint32_t getType() const {
		return type;
	}

	uint16_t getKeystroke() const {
		return keystroke;
	}
};