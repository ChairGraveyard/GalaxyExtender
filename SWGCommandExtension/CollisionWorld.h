#pragma once

#include "Object.h"
#include "soewrappers.h"

class CollisionWorld : public BaseHookedObject {
public:
	CollisionWorld() {

	}

	static void objectWarped(Object* object) {
		runStatic<0x012663E0, void>(object);
	}
};