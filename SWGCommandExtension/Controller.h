#pragma once

#include "Object.h"
#include "soewrappers.h"

#define APPENDMESSAGEADDRESS 0x00B3D200

class Controller : public BaseHookedObject {
public:
	constexpr static uint32_t RTTI = 0x0186AE78;

	Object* getOwner() {
		return getMemoryReference<Object*>(8);
	}

	void appendMessage(int message, float value, int flags) {
		runMethod<APPENDMESSAGEADDRESS, void>(message, value, flags);
	}
};