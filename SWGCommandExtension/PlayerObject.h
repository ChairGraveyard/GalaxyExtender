#pragma once

#include "IntangibleObject.h"

class PlayerObject : public IntangibleObject {
public:
	PlayerObject() {

	}

	bool speaksLanguage(int langid) const {
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0x65FEA0);
#else
		return ThisCall<0x65FEA0, bool, decltype(this), int>::run(this, langid);
#endif
	}
};