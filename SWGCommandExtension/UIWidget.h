#pragma once

#include "UIBaseObject.h"

class UIEventCallback;

class UIWidget : public UIBaseObject {
public:
	void SetFocus() {
		runMethod<0x1106BE0, void>();
	}

	void SetVisible(bool val) {
		runVirtual<0xB4, void>(val);
	}

	void SetEnabled(bool val) {
		runMethod<0x011067B0, void>(val);
	}

	void AddCallback(UIEventCallback* callback) {
		runMethod<0x01105D30, void>(callback);
	}
};