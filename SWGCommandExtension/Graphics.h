#pragma once


#include "Object.h"
#include "soewrappers.h"
#include "Controller.h"

#define SETFILLMODE 0x00755550

class Graphics : public BaseHookedObject {
public:
	static void setFillMode(int mode) {
		runStatic<SETFILLMODE, void>(mode);
	}
};