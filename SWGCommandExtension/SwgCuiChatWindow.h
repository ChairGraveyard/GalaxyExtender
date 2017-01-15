#pragma once

#include "Object.h"
#include "soewrappers.h"

#define CUICHATWINDOWCTOR 0x00F364B0

class SwgCuiChatWindow : public BaseHookedObject {
public:
	SwgCuiChatWindow* ctorHook() {
		originalCtor::run(this);

		return this;
	}

	DEFINE_HOOK(CUICHATWINDOWCTOR, ctorHook, originalCtor);
};