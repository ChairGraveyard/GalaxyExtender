#pragma once

#include "Object.h"
#include "soewrappers.h"

class ObjectAttributeManager {
public:
	static void formatDebugInfo(const Object* object, soe::unicode& str) {
		Call<0x4FBF50, void, const Object*, soe::unicode&>::run(object, str);
	}
};