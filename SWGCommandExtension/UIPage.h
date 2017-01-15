#pragma once

#include "UIBaseObject.h"

class UIPage : public UIBaseObject {
public:
	constexpr static uint32_t RTTI = 0x0189AEF0;

	static UIPage* DuplicateInto(UIPage& parent, const char* const path) {
		return runStatic<0x010FFF30, UIPage*, UIPage&, const char* const>(parent, path);
	}
};
