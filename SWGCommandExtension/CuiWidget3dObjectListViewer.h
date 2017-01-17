#pragma once

#include "UIWidget.h"

class CuiWidget3dObjectListViewer : public UIWidget {
public:
	void setObject(Object* obj) {
		runMethod<0x00A548F0, void>(obj);
	}
};