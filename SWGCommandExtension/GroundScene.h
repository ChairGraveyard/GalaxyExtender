#pragma once

#include "Object.h"
#include "soewrappers.h"
#include "FreeChaseCamera.h"

class Scene : public Object {
public:
	constexpr static uint32_t RTTI = 0x01868EF8;
};

class GroundScene : public Scene {
public:
	constexpr static uint32_t RTTI = 0x01868F0C;

	FreeChaseCamera* getFreeChaseCamera() {
		return getMemoryReference<FreeChaseCamera*>(0x6C);
	}
};
