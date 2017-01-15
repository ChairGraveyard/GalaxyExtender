#pragma once

#include "Object.h"

#include "soewrappers.h"
#include "Object.h"

class MessageQueue;

class InputMap : public BaseHookedObject {
public:
	MessageQueue* getMessageQueue() const {
		return getMemoryReference<MessageQueue*>(0xC);;
	}

	void handleInputReset() {
		runMethod<0x00AAE470, void>();
	}
};