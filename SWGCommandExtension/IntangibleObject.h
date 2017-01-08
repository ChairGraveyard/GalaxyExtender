#pragma once

#include "ClientObject.h"

class IntangibleObject : public ClientObject {
protected:
public:
	IntangibleObject() {

	}

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua 
			.beginNamespace("Game") //our defined namespace (w.e we want to call it) 
			.beginClass<IntangibleObject>("IntangibleObject") //define class object 
			.addConstructor<void(*)(void)>() //reg. empty constructor 
			.endClass() //end class 
			.endNamespace(); //end namespace
	}
};