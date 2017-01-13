#include "stdafx.h"

#include "TerrainObject.h"

float TerrainObject::newHighLoDValue = getHighLevelOfDetailThreshold();
float TerrainObject::newLoDThresholdValue = getLevelOfDetailThreshold();

DEFINE_HOOOK(HIGHLODADDRESS, TerrainObject::setHighLevelOfDetailThresholdHook, oldSetHighLoDThreshold);
DEFINE_HOOOK(SETLODADDRESS, TerrainObject::setLevelOfDetailThresholdHook, oldSetLoDThreshold);

void TerrainObject::setHighLevelOfDetailThresholdHook(float ) {
	oldSetHighLoDThreshold(newHighLoDValue);
}

void TerrainObject::setLevelOfDetailThresholdHook(float) {
	oldSetLoDThreshold(newLoDThresholdValue);
}