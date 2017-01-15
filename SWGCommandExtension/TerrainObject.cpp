#include "stdafx.h"

#include "TerrainObject.h"

float TerrainObject::newHighLoDValue = getHighLevelOfDetailThreshold();
float TerrainObject::newLoDThresholdValue = getLevelOfDetailThreshold();

SET_CLIENT_STATIC(TerrainObject::ms_instance, 0x01947194);

void TerrainObject::setHighLevelOfDetailThresholdHook(float ) {
	oldSetHighLoDThreshold::run(newHighLoDValue);
}

void TerrainObject::setLevelOfDetailThresholdHook(float) {
	oldSetLoDThreshold::run(newLoDThresholdValue);
}