#pragma once

#include "TangibleObject.h"
#include "CachedNetworkId.h"

#include <vector>

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

	const int* getAttributesArray() const {
		/* relative address of the vector storing the attributes, its inside another class AutoDeltaVector */
		return getMemoryReference<int* >(0x410);
	}

	int* getAttributesArray() {
		return getMemoryReference<int* >(0x410);
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

	ClientObject* getEquippedObject(const char* slotName) {
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0x431970);
#else
		return ThisCall<0x431970, ClientObject*, decltype(this), const char*>::run(this, slotName);
#endif
	}

	CachedNetworkId* getLookAtTarget() {
		return &getMemoryReference<CachedNetworkId>(0x598);
	}

	void setState(int8_t state, bool value) {
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0x4352C0);
#else
		ThisCall<0x4352C0, void, decltype(this)>::run(this);
#endif
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
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0x4328D0);
#else
		ThisCall<0x4328D0, void, decltype(this), int8_t>::run(this, posture);
#endif
	}

	void requestServerPostureChange(int8_t posture) {
#ifdef USEJUMPTOCLIENTHACK
		JUMPTOCLIENT(0x432860);
#else
		ThisCall<0x432860, void, decltype(this), int8_t>::run(this, posture);
#endif
	}
};