#pragma once

#include <stdio.h>
#include <string>

#include "Object.h"

class CachedNetworkId {
protected:
	uint64_t oid = 0;

	/*different Bla<Object>*/
	uint32_t* possibleVtable = nullptr;
	Object* data = nullptr;

public:
	CachedNetworkId() = default;
	CachedNetworkId(const CachedNetworkId&) = default;

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