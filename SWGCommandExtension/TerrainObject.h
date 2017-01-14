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
public:
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

	DEFINE_HOOOK(HIGHLODADDRESS, setHighLevelOfDetailThresholdHook, oldSetHighLoDThreshold);
	DEFINE_HOOOK(SETLODADDRESS, setLevelOfDetailThresholdHook, oldSetLoDThreshold);

	static float getHighLevelOfDetailThreshold() {
		return Call<GETHIGHLOADADDRESS, float>::run();
	}

	static float getLevelOfDetailThreshold() {
		return Call<GETLODADDRESS, float>::run();
	}
};
