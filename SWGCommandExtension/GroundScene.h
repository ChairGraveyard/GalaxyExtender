#pragma once

#include "Object.h"
#include "soewrappers.h"
#include "FreeChaseCamera.h"

#define PARSEMESGADDRESS 0x0051A900
#define SETVIEWADDRESS 0x0051A350

class InputMap;

class Scene : public BaseHookedObject {
public:
	constexpr static uint32_t RTTI = 0x01868EF8;
};

class GroundScene : public Scene {	
public:
	constexpr static uint32_t RTTI = 0x01868F0C;

	FreeChaseCamera* getFreeChaseCamera() {
		return getMemoryReference<FreeChaseCamera*>(0x6C);
	}

	int getCurrentView() {
		return getMemoryReference<int>(0x9C);
	}

	Object* getCamera() {
		return runMethod<0x0051A490, Object*>();
	}

	Object* getCurrentCamera() {
		return runMethod<0x0051A4D0, Object*>();
	}

	void activateGodClientInteriorCamera() {
		getMemoryReference<bool>(0xA2) = true;
	}

	void setView(int newView, float val = 0) {
		runMethod<SETVIEWADDRESS, void>(newView, val);
	}

	void reloadTerrain() {
		runMethod<0x0051A4F0, void>();
	}

	void load(const char* terrainFileName, float timeSeconds) {
		runMethod<0x0051A5A0, void>(terrainFileName, timeSeconds);
	}

	void parseMessages(InputMap* map);

	DEFINE_HOOK(PARSEMESGADDRESS, parseMessages, originalParseMessages);
};
