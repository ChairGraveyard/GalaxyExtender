#pragma once

#include "Object.h"
#include "soewrappers.h"

#define LOGINSCREEN_BUTTON_PRESSED 0xC8D460

class SwgCuiLoginScreen : public Object {
public:
	void onButtonPressed(Object* uicontext);
	
	DEFINE_HOOOK(LOGINSCREEN_BUTTON_PRESSED, onButtonPressed, onButtonPressedOriginal);
};
