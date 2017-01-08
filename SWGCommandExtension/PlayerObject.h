#pragma once

#include "IntangibleObject.h"

class PlayerObject : public IntangibleObject {
public:
	PlayerObject() {

	}

	bool speaksLanguage(int langid) const {
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0x65FEA0);
#else
		return ThisCall<0x65FEA0, bool, decltype(this), int>::run(this, langid);
#endif
	}

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua 
			.beginNamespace("Game") //our defined namespace (w.e we want to call it) 
			.beginClass<PlayerObject>("PlayerObject") //define class object 
			.addConstructor<void(*)(void)>() //reg. empty constructor 
			.addFunction("speaksLanguage", &PlayerObject::speaksLanguage)
			.endClass() //end class 
			.endNamespace(); //end namespace
	}
};