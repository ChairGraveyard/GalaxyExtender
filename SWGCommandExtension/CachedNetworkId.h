#pragma once

#include <stdio.h>
#include <string>

#include "NetworkId.h"
#include "Object.h"

class CachedNetworkId {
protected:
	NetworkId oid;

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

	const NetworkId& getObjectID() const {
		return oid;
	}

	NetworkId& getObjectID() {
		return oid;
	}
};