#pragma once

#include "IntangibleObject.h"

class PlayerObject : public IntangibleObject {
public:
	PlayerObject() {

	}

	bool speaksLanguage(int langid) const {
		return runMethod<0x65FEA0, bool>(langid);
	}
};