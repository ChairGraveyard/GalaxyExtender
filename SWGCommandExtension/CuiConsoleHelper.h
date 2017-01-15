#pragma once

#include "UIEventCallback.h"

class UIPage;
class UIText;
class UITextbox;
class CuiConsoleHelperParserStrategy;
class CommandParserHistory;

class CuiConsoleHelper : public UIEventCallback {
	char data[0x38];
public:
	void ctor(UIPage* containerPage, UIText* text, UITextbox* textbox, const CuiConsoleHelperParserStrategy * strategy, CommandParserHistory * history) {
		runMethod<0x00913930, void>(containerPage, text, textbox, strategy, history);
	}
};