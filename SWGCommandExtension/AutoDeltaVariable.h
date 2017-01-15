#pragma once

#include "soewrappers.h"
#include "Object.h"

template<typename T>
class AutoDeltaVariable : public BaseHookedObject {
public:
	const T& getCurrent() const {
		return getMemoryReference<T>(0xC);
	}

	const T& getLast() const {
		return getMemoryReference<T>(0x10);
	}

	void set(const T& val) {
		if (getCurrent() != val) {
			runVirtual<0x1C, void, const T&>(val);
		}
	}
};
