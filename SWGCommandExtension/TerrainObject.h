#pragma once

#include "Object.h"
#include "soewrappers.h"

#define HIGHLODADDRESS 0x00B5CCA0
#define GETHIGHLOADADDRESS 0x00B5CC80
#define GETLODADDRESS 0x00B5CCC0
#define SETLODADDRESS 0x00B5CCE0

class TerrainObject : public Object {
	static float newHighLoDValue;
	static float newLoDThresholdValue;

	DEFINE_CLIENT_STATIC(TerrainObject*, ms_instance);

public:
	static TerrainObject* getInstance() {
		return ms_instance;
	}

	static void setHighLevelOfDetailThreshold(float newValue) {
		newHighLoDValue = newValue;

		setHighLevelOfDetailThresholdHook(newHighLoDValue);
	}

	static void setLevelOfDetailThreshold(float newValue) {
		newLoDThresholdValue = newValue;

		setLevelOfDetailThresholdHook(newLoDThresholdValue);
	}
		
	static void setHighLevelOfDetailThresholdHook(float levelOfDetail);
	static void setLevelOfDetailThresholdHook(float levelOfDetail);

	DEFINE_HOOK(HIGHLODADDRESS, setHighLevelOfDetailThresholdHook, oldSetHighLoDThreshold);
	DEFINE_HOOK(SETLODADDRESS, setLevelOfDetailThresholdHook, oldSetLoDThreshold);

	bool isWithinTerrainBoundaries(const Vector& position) const {
		return runMethod<0x00B5CD90, bool>(position);
	}

	float getMapWidthInMeters() const {
		return runMethod<0x00B5C3C0, float>();
	}

	float getMaximumValidHeightInMeters() const {
		return runMethod<0x00B5C3D0, float>();
	}

	static float getHighLevelOfDetailThreshold() {
		return Call<GETHIGHLOADADDRESS, float>::run();
	}

	static float getLevelOfDetailThreshold() {
		return Call<GETLODADDRESS, float>::run();
	}
};
