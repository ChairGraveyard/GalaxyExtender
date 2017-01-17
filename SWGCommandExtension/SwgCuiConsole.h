#pragma once
#include "CuiMediator.h"

class UIPage;
class CommandParserHistory;
class CuiChatParserStrategy;
class UITextbox;
class UIText;
class CuiConsoleHelper;
class CuiWidget3dObjectListViewer;
class Object;

class SwgCuiConsole : public CuiMediator {
	char fullData[0xC8]; //fill enough for cuimediator

	UIText* outputText = nullptr;
	UITextbox* inputTextbox = nullptr;

	CommandParserHistory* history = nullptr;
	CuiChatParserStrategy* parserStrategy = nullptr;
	CuiConsoleHelper* consoleHelper = nullptr;
	CuiWidget3dObjectListViewer* viewer3D = nullptr;

	DEFINE_VTABLE(newVtable); 
public:
	void ctor(UIPage& page);
	void initializeVtable();

	void performActivate();
	DEFINE_VMETHOD(0x24, performActivate);

	void performDeactivate();

	void set3DObject(Object* obj);
};
