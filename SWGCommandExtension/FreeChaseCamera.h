#pragma once

#include "Object.h"
#include "soewrappers.h"

#define VIEW_DISTANCE_OFFSET 0xFC

class FreeChaseCamera : public Object {
public:
	float getViewDistance() {
		return getMemoryReference<float>(VIEW_DISTANCE_OFFSET);
	}

	void setViewDistance(float value) {
		getMemoryReference<float>(VIEW_DISTANCE_OFFSET) = value;
	}
};