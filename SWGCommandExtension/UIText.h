#pragma once

#include "UIWidget.h"

class UIText : public UIWidget {
public:
	void AppendLocalText(const soe::unicode& str) {
		runMethod<0x0110FB40, void, const soe::unicode&>(str);
	}
};