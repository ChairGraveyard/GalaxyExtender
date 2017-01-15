#pragma once

#include "soewrappers.h"
#include "Object.h"

class UIPage;

class UIManager : public BaseHookedObject {
public:
	UIPage* GetRootPage() {
		return getMemoryReference<UIPage*>(0x4);
	}

	static UIManager* gUIManager() {
		return runStatic<0x0087FF60, UIManager*>();
	}
};