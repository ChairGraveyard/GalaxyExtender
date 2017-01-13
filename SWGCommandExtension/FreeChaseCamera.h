#pragma once

#include "Object.h"
#include "soewrappers.h"

#define VIEW_DISTANCE_OFFSET 0xFC

class FreeChaseCamera : public Object {
public:
	const float& getViewDistance() const {
		return getMemoryReference<float>(VIEW_DISTANCE_OFFSET);
	}
	
	float& getViewDistance() {
		return getMemoryReference<float>(VIEW_DISTANCE_OFFSET);
	}

	void setViewDistance(float value) {
		getViewDistance() = value;
	}
};