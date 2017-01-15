#pragma once

#include "Object.h"
#include "soewrappers.h"

#define LOGINSCREEN_BUTTON_PRESSED 0xC8D460

class SwgCuiLoginScreen : public BaseHookedObject {
public:
	void onButtonPressed(Object* uicontext);
	
	DEFINE_HOOK(LOGINSCREEN_BUTTON_PRESSED, onButtonPressed, onButtonPressedOriginal);
};
