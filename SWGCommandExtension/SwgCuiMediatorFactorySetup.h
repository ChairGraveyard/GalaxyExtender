#pragma once

#include "Object.h"

class SwgCuiMediatorFactorySetup : public BaseHookedObject {
public:
	static void install();

	DEFINE_HOOK(0x00BBA510, install, originalInstall);
};