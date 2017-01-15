#pragma once

#include "soewrappers.h"
#include "Object.h"

class UIBaseObject : public BaseHookedObject {
public:
	constexpr static uint32_t RTTI = 0x0189A8A8;

	UIBaseObject* GetObjectFromPath(const char* str, int type) const {
		return runMethod<0x010F5020, UIBaseObject*>(str, type);
	}

	UIBaseObject* GetObjectFromPath(const char* const name) const {
		return runMethod<0x010F4FA0, UIBaseObject*>(name);
	}

	UIBaseObject* GetRoot() const {
		return runMethod<0x010F4E30, UIBaseObject*>();
	}

	bool IsA(int type) {
		return runVirtual<0, bool, int>(type);
	}
};