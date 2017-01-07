#pragma once

#include <string.h>
#include <cstdint>

//complete size 0x3C8
class TangibleObject {
protected:
	uint8_t clientTangibleObjectData[0x3C8]; //this includes data from inherited classes (ClientObject, CallbackReceiver)
public:
	TangibleObject() {
		memset(clientTangibleObjectData, 0, sizeof(clientTangibleObjectData));
	}

	TangibleObject(void* templateData) {
		//address is 63EE10
	}

	template<typename T>
	const T& getMemoryReference(int offset) const {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	template<typename T>
	T& getMemoryReference(int offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uint32_t>(this) + offset);
	}

	int getCondition() const {
		return getMemoryReference<int>(0x38C);
	}

};
