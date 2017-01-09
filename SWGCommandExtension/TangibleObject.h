#pragma once

#include "ClientObject.h"

//complete size 0x3C8
class TangibleObject : public ClientObject {
protected:
	uint8_t clientTangibleObjectData[0x3C8]; //this includes data from inherited classes (ClientObject, CallbackReceiver)
public:
	TangibleObject() {
		memset(clientTangibleObjectData, 0, sizeof(clientTangibleObjectData));
	}

	TangibleObject(void* templateData) {
		//address is 63EE10
	}

	int getConditionBitmask() const {
		return getMemoryReference<int>(0x38C);
	}

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua 
			.beginNamespace("Game") //our defined namespace (w.e we want to call it) 
			.beginClass<TangibleObject>("TangibleObject") //define class object 
			.addConstructor<void(*)(void)>() //reg. empty constructor 
			.endClass() //end class 
			.endNamespace(); //end namespace
	}
};
