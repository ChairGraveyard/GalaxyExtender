#pragma once

#include "soewrappers.h"
#include "Object.h"

class Message {
public:
	int message;
	float value;
	void* data;
	uint32_t flags;
};

class MessageQueue : public BaseHookedObject {
public:
	void MessageQueue::getMessage(int index, int* message, float* value, uint32_t* flags = nullptr) {
		runMethod<0x00AA63B0, void>(index, message, value, flags);
	}

	std::size_t getNumberOfMessages() const {
		return runMethod<0x00AA6660, std::size_t>();
	}
};