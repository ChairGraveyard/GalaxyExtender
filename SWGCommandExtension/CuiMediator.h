#pragma once

#include "soewrappers.h"
#include "Object.h"

class UIPage;

class CuiMediator : public BaseHookedObject {
public:
	void ctor(const char* name, UIPage& page) {
		runMethod<0x009BFF00, CuiMediator*, const char*, UIPage&>(name, page);
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
};