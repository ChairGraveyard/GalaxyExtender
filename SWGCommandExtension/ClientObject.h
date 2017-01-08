#pragma once

#include "Object.h"
#include "stlwrappers.h"

#include "LuaEngine.h"
#include "LuaBridge\LuaBridge.h"

class ClientObject : public Object {
public:
	ClientObject() {

	}

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua 
			.beginNamespace("Game") //our defined namespace (w.e we want to call it) 
			.beginClass<ClientObject>("ClientObject") //define class object 
			.addConstructor<void(*)(void)>() //reg. empty constructor 
			.endClass() //end class 
			.endNamespace(); //end namespace
	}
};