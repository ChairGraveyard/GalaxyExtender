#pragma once

#include "TangibleObject.h"

#include "utility.h"

#include "LuaBridge\LuaBridge.h"



//complete size 0x9B8
class CreatureObject : public TangibleObject {
protected:
	uint8_t clientCreatureObjectData[0x5f0]; /*this is to at least make the compiler calculate the right sizeof of the object*/
public:
	enum Attribute { Health = 0, Strength, Constitution, Action, Quickness, Stamina, Mind, Focus, Willpower };

	CreatureObject() {
		memset(clientCreatureObjectData, 0, sizeof(clientCreatureObjectData));
	}

	CreatureObject(void* templateData) {
		//address is 0042ECF0
	}

	void setAttribute(Attribute i, int value) {
		getAttributesArray()[i] = value;
	}

	int getAttribute(Attribute i) const {
		return getAttributesArray()[i];
	}

	const std::vector<int>& getAttributesArray() const {
		/* relative address of the vector storing the attributes, its inside another class AutoDeltaVector */
		return getMemoryReference<std::vector<int> >(0x410);
	}

	std::vector<int>& getAttributesArray() {
		return getMemoryReference<std::vector<int> >(0x410);
	}

	bool CreatureObject::isRidingMount() {
		return getMemoryReference<bool>(0x98D);
	}

	uint8_t getMood() {
		return getMemoryReference<uint8_t>(0x8D0);
	}

	bool getState(uint8_t state) {
		uint64_t stateBitmask = getMemoryReference<uint64_t>(0x758);

		return (stateBitmask & (1i64 << state)) != 0;
	}

	int8_t getVisualPosture() const {
		return getMemoryReference<int8_t>(0x3D4);
	}

	int8_t getServerPosture() const {
		return getMemoryReference<int8_t>(0x3EC);
	}	
	
	// TODO: Replace with nice Point3F, MatrixF for convenience & calculations.
	float getXLocation() const {
		return getMemoryReference<float>(0x5C);
	}	
	
	float getYLocation() const {
		return getMemoryReference<float>(0x6C);
	}

	float getZLocation() const {
		return getMemoryReference<float>(0x7C);
	}

	bool isSittingOnObject() const {
		return getMemoryReference<bool>(0x8D9);
	}

	void setSittingOnObject(bool val) {
		getMemoryReference<bool>(0x8D9) = val;
	}

	void setVisualPosture(int8_t posture) {
		JUMPTOCLIENT(0x4328D0);
	}

	static void register_lua(lua_State* L)
	{
		using namespace luabridge;
		getGlobalNamespace(L) //global namespace to lua
			.beginNamespace("Game") //our defined namespace (w.e we want to call it)
			.beginClass<CreatureObject>("CreatureObject") //define class object
			.addConstructor<void(*)(void)>() //reg. empty constructor
			.addFunction("getMood", &CreatureObject::getMood) 
			.addFunction("getAttribute", &CreatureObject::getAttribute)
			.endClass() //end class
			.endNamespace(); //end namespace		
	}
};


