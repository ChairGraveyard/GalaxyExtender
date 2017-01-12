#pragma once

#include "Object.h"
#include "soewrappers.h"

#define LOGINSCREEN_ACTIVATE 0xC8D190
typedef void(__thiscall* loginScreenActivate_t)(void*);
loginScreenActivate_t originalLoginScreenActivateObject = (loginScreenActivate_t)LOGINSCREEN_ACTIVATE; //i patch this one on startup, no hooks needed

#define LOGINSCREEN_BUTTON_PRESSED 0xC8D460
typedef void(__thiscall* loginScreenOnButtonPressed_t)(void*, void*);
loginScreenOnButtonPressed_t originalLoginScreenOnButtonPressedObject = (loginScreenOnButtonPressed_t)LOGINSCREEN_BUTTON_PRESSED;

#define GAME_APPLICATION_ADDRES 0x01908858

void __fastcall hkLoginScreenOnButtonPressed(Object* thisPointer, void* unused, void* uicontext) {
	/*char message[128];
	int* gameType = (int*)GAME_APPLICATION_ADDRES;

	sprintf_s(message, "this:%p uicontext:%p applicationType:%d", thisPointer, uicontext, *gameType);
	OutputDebugStringA(message);*/

	originalLoginScreenOnButtonPressedObject(thisPointer, uicontext);
}

class SwgCuiLoginScreen : public Object {
public:

};