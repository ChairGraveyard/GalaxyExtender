#pragma once
#include "CuiMediator.h"
#include "UIEventCallback.h"

class UIMessage;
class UIPage;
class CommandParserHistory;
class CuiChatParserStrategy;
class UITextbox;
class UIText;
class CuiConsoleHelper;
class CuiWidget3dObjectListViewer;
class Object;

class SwgCuiConsole : public CuiMediator, public UIEventCallback {
	char fullData[0xC8 * 2]; //fill enough for cuimediator

	UIText* outputText = nullptr;
	UITextbox* inputTextbox = nullptr;

	CommandParserHistory* history = nullptr;
	CuiChatParserStrategy* parserStrategy = nullptr;
	CuiConsoleHelper* consoleHelper = nullptr;
	CuiWidget3dObjectListViewer* viewer3D = nullptr;

	UIPage* thePage = nullptr;

	DEFINE_VTABLE(newVtableMediator); 
	DEFINE_VTABLE(newVtableEventCallback);

public:
	void ctor(UIPage& page);
	void initializeVtable();

	void performActivate();
	DEFINE_VMETHOD(0x24, performActivate);

	bool OnMessage(UIWidget* context, const UIMessage* msg);
	DEFINE_VMETHOD(0, OnMessage);

	void performDeactivate();

	void set3DObject(Object* obj);

	UIEventCallback* getCallbackObject() {
		return (UIEventCallback*)(((char*)this) + 0x84);
	}

	DEFINE_VMETHOD(0x30, getCallbackObject);
	

	void registerMediatorObject(UIBaseObject* obj, bool activeCallbacks);
};
