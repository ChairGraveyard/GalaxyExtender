#include "stdafx.h"

#include "UIText.h"
#include "UITextbox.h"
#include "SwgCuiConsole.h"
#include "UIPage.h"
#include "CommandParserHistory.h"
#include "CuiChatParserStrategy.h"
#include "Game.h"
#include "SwgCuiCommandParserDefault.h"
#include "CuiConsoleHelper.h"

uint32_t SwgCuiConsole::newVtable[] = { 0x161D254, 0x9C0FB0, 0x9C1000, 0x9C1010, 0x9C1360, 0x9C1040, 0x9C1CC0, 0x9C1D30, 0x9C2760, 0x9C05C0, 0x9C0D50, 0x9C0D60, 0x9C0B80 };

void SwgCuiConsole::ctor(UIPage& page) {
	CuiMediator::ctor("SwgCuiConsole", page);

	//set pointer of the vftable to our own
	*((uint32_t**)this) = newVtable + 1; //first value is the rtti pointer

										 //update vtable to our own pointer
	performActivate_hook_t::hookStorage_t::newMethod = &SwgCuiConsole::performActivate;

	newVtable[10] = *((uint32_t*)((void*)&performActivate_hook_t::hookStorage_t::newMethod));

	//now we run our stuff
	m_outputText = (UIText*)page.GetObjectFromPath("OutputText", 35);
	m_inputTextbox = (UITextbox*)page.GetObjectFromPath("InputTextbox", 36);

	m_history = new CommandParserHistory(); //this is gonna leak untill i hook the dtor in the vtable
	auto parser = create_hooked_object<SwgCuiCommandParserDefault>(m_history);
	m_parserStrategy = create_hooked_object<CuiChatParserStrategy>(parser, false);
	m_consoleHelper = create_hooked_object<CuiConsoleHelper>(&page, m_outputText, m_inputTextbox, m_parserStrategy, m_history);

	m_outputText->AppendLocalText("riiiiise");
}

void SwgCuiConsole::performActivate() {
	setPointerInputActive(true);
	setKeyboardInputActive(true);
	setInputToggleActive(false);
}