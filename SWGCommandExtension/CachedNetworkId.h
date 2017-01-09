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

	const uint64_t& _getObjectID() const {
		return oid;
	}

	uint64_t& getObjectID() {
		return oid;
	}

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua 
			.beginNamespace("Game") //our defined namespace (w.e we want to call it) 
			.beginClass<CachedNetworkId>("CachedNetworkId") //define class object 
			//.addConstructor<void(*)(void)>() //reg. empty constructor 
			.addFunction("getObject", &CachedNetworkId::getObject)	
			.addFunction("getObjectID", &CachedNetworkId::_getObjectID) // TODO: Figure out how to specify which function to use among overloaded ones.
			.endClass() //end class 
			.endNamespace(); //end namespace
	}
};