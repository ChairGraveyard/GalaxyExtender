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
#include "CuiWidget3dObjectListViewer.h"
#include "EmuCommandParser.h"

uint32_t SwgCuiConsole::newVtable[] = { 0x161D254, 0x9C0FB0, 0x9C1000, 0x9C1010, 0x9C1360, 0x9C1040, 0x9C1CC0, 0x9C1D30, 0x9C2760, 0x9C05C0, 0x9C0D50, 0x9C0D60, 0x9C0B80 };

void SwgCuiConsole::ctor(UIPage& page) {
	CuiMediator::ctor("SwgCuiConsole", page);

	//set pointer of the vftable to our own
	*((uint32_t**)this) = newVtable + 1; //first value is the rtti pointer

										 //update vtable to our own pointer
	performActivate_hook_t::hookStorage_t::newMethod = &SwgCuiConsole::performActivate;

	newVtable[10] = *((uint32_t*)((void*)&performActivate_hook_t::hookStorage_t::newMethod));

	//now we run our stuff
	outputText = (UIText*)page.GetObjectFromPath("OutputText", 35);
	inputTextbox = (UITextbox*)page.GetObjectFromPath("InputTextbox", 36);
	viewer3D = (CuiWidget3dObjectListViewer*) page.GetObjectFromPath("v");

	history = new CommandParserHistory(); //this is gonna leak untill i hook the dtor in the vtable
	auto parser = create_soe_object<SwgCuiCommandParserDefault>(history);

	parser->addSubCommand(create_soe_object<EmuCommandParser>());

	parserStrategy = create_soe_object<CuiChatParserStrategy>(parser, false);
	consoleHelper = create_soe_object<CuiConsoleHelper>(&page, outputText, inputTextbox, parserStrategy, history);

	consoleHelper->setEcho(true);

	soe::unicode str("\\#ffffff");

	outputText->AppendLocalText(str);

	soe::unicode helpString;
	EmuCommandParser::showHelp(helpString);

	outputText->AppendLocalText(helpString);
	outputText->AppendLocalText(str);
}

void SwgCuiConsole::performActivate() {
	setPointerInputActive(true);
	setKeyboardInputActive(true);
	setInputToggleActive(false);

	//viewer3D->SetEnabled(true);
	//viewer3D->SetVisible(true);

	inputTextbox->SetFocus();
}

void SwgCuiConsole::performDeactivate() {

}

void SwgCuiConsole::set3DObject(Object* obj) {
	//viewer3D->setObject(obj);
}