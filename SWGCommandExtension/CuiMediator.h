#pragma once

#include "soewrappers.h"
#include "Object.h"

#include "UIWidget.h"

class UIPage;
class UIWidget;
class UIBaseObject;

class CuiMediator : public BaseHookedObject {
public:
	typedef std::pair<UIBaseObject*, bool> ObjectCallbackData;

	void ctor(const char* name, UIPage& page) {
		runMethod<0x009BFF00, CuiMediator*, const char*, UIPage&>(name, page);
	}

	void registerMediatorObject(UIBaseObject& obj, bool callbacks) {
		runMethod<0x009C18A0, void, UIBaseObject&, bool>(obj, callbacks);
	}

	UIWidget* GetParentWidget() {
		return runMethod<0x0110C450, UIWidget*>();
	}

	void fetch() {
		runMethod<0x009C1480, void>();
	}

	void release() {
		runMethod<0x09C1490, void>();
	}

	void setInputToggleActive(bool val) {
		runMethod<0x9C0D70, void>(val);
	}

	void setKeyboardInputActive(bool val) {
		runMethod<0x9C0E00, void>(val);
	}

	void setPointerInputActive(bool val) {
		runMethod<0x9C0E40, void>(val);
	}

	const uint32_t& getStates() const {
		return getMemoryReference<uint32_t>(0x1C);
	}

	bool isActive() const {
		return (getStates() & 0x800) != 0;
	}

	soe::vector<ObjectCallbackData>* getCallbackVector() {
		return getMemoryReference<soe::vector<ObjectCallbackData>*>(0x6C);
	}

};