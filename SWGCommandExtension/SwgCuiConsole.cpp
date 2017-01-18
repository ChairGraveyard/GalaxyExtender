#include "stdafx.h"

#include <cassert>

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
#include "UIMessage.h"
#include "CuiMediatorFactory.h"

uint32_t SwgCuiConsole::newVtableMediator[13]; /* for reference { 0x161D254, 0x9C0FB0, 0x9C1000, 0x9C1010, 0x9C1360, 0x9C1040, 0x9C1CC0, 0x9C1D30, 0x9C2760, 0x9C05C0, 0x9C0D50, 0x9C0D60, 0x9C0B80 };*/
uint32_t SwgCuiConsole::newVtableEventCallback[33];

void SwgCuiConsole::initializeVtable() {
	INITIALIZE_VTABLE_DATA(newVtableMediator);

	SETVTABLEENTRY(newVtableMediator, SwgCuiConsole::performActivate);
	SETVTABLEENTRY(newVtableMediator, SwgCuiConsole::getCallbackObject);

	SETVTABLE(newVtableMediator);

	INITIALIZE_VTABLE_DATAFROM(newVtableEventCallback, 0x015E7F04);

	SETVTABLEENTRY(newVtableEventCallback, SwgCuiConsole::OnMessage);

	SETVTABLEOFFSET(newVtableEventCallback, 0x84);
}

void SwgCuiConsole::ctor(UIPage& page) {
	CuiMediator::ctor("SwgCuiConsole", page);

	//now we run our stuff
	outputText = (UIText*)page.GetObjectFromPath("OutputText", 35);
	inputTextbox = (UITextbox*)page.GetObjectFromPath("InputTextbox", 36);
	viewer3D = (CuiWidget3dObjectListViewer*) page.GetObjectFromPath("v");
	thePage = &page;

	assert(outputText && inputTextbox && viewer3D);

	history = new CommandParserHistory(); //this is gonna leak untill i hook the dtor in the vtable
	auto parser = create_soe_object<SwgCuiCommandParserDefault>(history);

	parser->addSubCommand(create_soe_object_vtable<EmuCommandParser>());

	parserStrategy = create_soe_object<CuiChatParserStrategy>(parser, false);
	consoleHelper = create_soe_object<CuiConsoleHelper>(&page, outputText, inputTextbox, parserStrategy, history);

	consoleHelper->setEcho(true);

	soe::unicode str("\\#ffffff");

	outputText->AppendLocalText(str);

	soe::unicode helpString;
	EmuCommandParser::showHelp(helpString);

	outputText->AppendLocalText(helpString);
	outputText->AppendLocalText(str);

	registerMediatorObject(thePage, true);

	//OutputDebugStringA(std::string("this pointer:" + std::to_string((uint32_t)this)).c_str());
}

void SwgCuiConsole::performActivate() {
	OutputDebugStringA("activating swg cui console");

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

bool SwgCuiConsole::OnMessage(UIWidget* context, const UIMessage* msg) {
	//OutputDebugStringA(std::string("this OnMessage pointer:" + std::to_string((uint32_t)this)).c_str());
	//this pointer is 0x84 because we're overriding UIEventCallback
	
	int stroke = msg->getKeystroke();

	/*OutputDebugStringA("OnMessage Keystroke");
	OutputDebugStringA(std::to_string(stroke).c_str());*/

	if (stroke == 27) { //escape key
		CuiMediatorFactory::toggle("Console");

		return false;
	}

	return true;
}

void SwgCuiConsole::registerMediatorObject(UIBaseObject* obj, bool activeCallbacks) {
	obj->Attach(nullptr);

	auto callbackVector = getCallbackVector();
	callbackVector->push_back(ObjectCallbackData(obj, activeCallbacks));

	//if (isActive()) {
		UIWidget* widget = (UIWidget*)(obj);

		auto test = getCallbackObject();

		widget->AddCallback(test);
	//}

}