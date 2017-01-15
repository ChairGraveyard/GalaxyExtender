#pragma once
#include "CuiMediator.h"

class UIPage;
class CommandParserHistory;
class CuiChatParserStrategy;
class UITextbox;
class UIText;
class CuiConsoleHelper;

class SwgCuiConsole : public CuiMediator {
	char fullData[0xC8]; //fill enough for cuimediator

	UIText* m_outputText = nullptr;
	UITextbox* m_inputTextbox = nullptr;

	CommandParserHistory* m_history = nullptr;
	CuiChatParserStrategy* m_parserStrategy = nullptr;
	CuiConsoleHelper* m_consoleHelper = nullptr;

	static uint32_t newVtable[];
public:
	void ctor(UIPage& page);

	void performActivate();
	DEFINE_HOOK(1, performActivate, originalActivate);

	
};
