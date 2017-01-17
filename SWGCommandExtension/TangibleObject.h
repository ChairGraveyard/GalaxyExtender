#pragma once

#include "ClientObject.h"

//complete size 0x3C8
class TangibleObject : public ClientObject {
protected:
	uint8_t clientTangibleObjectData[0x3C8]; //this includes data from inherited classes (ClientObject, CallbackReceiver)
public:
	TangibleObject() {
		memset(clientTangibleObjectData, 0, sizeof(clientTangibleObjectData));
	}

	TangibleObject(void* templateData) {
		//address is 63EE10
	}

	int getConditionBitmask() const {
		return getMemoryReference<int>(0x38C);
	}

};
