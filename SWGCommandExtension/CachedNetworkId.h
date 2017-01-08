#pragma once

#include <stdio.h>
#include <string>

#include "Object.h"

class CachedNetworkId {
protected:
	uint64_t oid;
	Object* data;

public:
	Object* getObject() {
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0xB30160);
#else
		return ThisCall<0xB30160, Object*, CachedNetworkId*>::run(this);
#endif
	}

	const uint64_t& getObjectID() const {
		return oid;
	}

	uint64_t& getObjectID() {
		return oid;
	}
};