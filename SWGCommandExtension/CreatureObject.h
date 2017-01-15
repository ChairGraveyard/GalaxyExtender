#pragma once

#include "TangibleObject.h"
#include "CachedNetworkId.h"
#include "soewrappers.h"
#include "AutoDeltaVariable.h"

#include <vector>

#define PTR_TO_CREATUREOBJECT_TYPINFO 0x01869148

//complete size 0x9B8
class CreatureObject : public TangibleObject {
protected:
	uint8_t clientCreatureObjectData[0x5f0]; /*this is to at least make the compiler calculate the right sizeof of the object*/
public:
	constexpr static uint32_t RTTI = PTR_TO_CREATUREOBJECT_TYPINFO;

	enum Attribute { Health = 0, Strength, Constitution, Action, Quickness, Stamina, Mind, Focus, Willpower };

	CreatureObject() {
		memset(clientCreatureObjectData, 0, sizeof(clientCreatureObjectData));
	}

	CreatureObject(void* templateData) {
		//address is 0042ECF0
	}

	int getAttribute(Attribute i) const {
		return getAttributesArray()[i];
	}

	const soe::vector<int>& getAttributesArray() const {
		/* relative address of the vector storing the attributes, its inside another class AutoDeltaVector */
		return getMemoryReference<soe::vector<int> >(0x410);
	}

	bool CreatureObject::isRidingMount() const {
		return getMemoryReference<bool>(0x98D);
	}

	uint8_t getMood() const {
		return getMemoryReference<uint8_t>(0x8D0);
	}

	bool getState(uint8_t state) const {
		uint64_t stateBitmask = getMemoryReference<uint64_t>(0x758);

		return (stateBitmask & (1i64 << state)) != 0;
	}

	ClientObject* getEquippedObject(const char* slotName) {
		return runMethod<0x431970, ClientObject*>(slotName);
	}

	CachedNetworkId& getLookAtTarget() {
		return getMemoryReference<CachedNetworkId>(0x598);
	}

	void setState(int8_t state, bool value) {
		runMethod<0x4352C0, void>(state, value);
	}

	int8_t getVisualPosture() const {
		return getMemoryReference<int8_t>(0x3D4);
	}

	int8_t getServerPosture() const {
		return getMemoryReference<int8_t>(0x3EC);
	}

	bool isSittingOnObject() const {
		return getMemoryReference<bool>(0x8D9);
	}

	void setSittingOnObject(bool val) {
		getMemoryReference<bool>(0x8D9) = val;
	}

	void setVisualPosture(int8_t posture) {
		runMethod<0x4328D0, void>(posture);
	}

	void requestServerPostureChange(int8_t posture) {
		runMethod<0x432860, void>(posture);
	}

	//------------

	//-----------
};