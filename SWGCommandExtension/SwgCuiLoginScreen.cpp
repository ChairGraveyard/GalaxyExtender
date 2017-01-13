#include "stdafx.h"

#include "SwgCuiLoginScreen.h"
#include "soewrappers.h"

#define LOGINSCREEN_ACTIVATE 0xC8D190
#define GAME_APPLICATION_ADDRES 0x01908858

SET_HOOK_THISCALL(SwgCuiLoginScreen, onButtonPressed, void, OnButtonPressedHookObject, Object*);

void SwgCuiLoginScreen::onButtonPressed(Object* uicontext) {
	OnButtonPressedHookObject::runOriginal(this, uicontext);
}
