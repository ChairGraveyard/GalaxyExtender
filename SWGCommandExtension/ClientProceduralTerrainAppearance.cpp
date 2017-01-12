#include "stdafx.h"

#include "ClientProceduralTerrainAppearance.h"

#define GET_NONCOLLIDABLE_FLORA_ADDRESS_PTR 0x01924B94
#define RADIAL_FLORA_ADDRESS 0x01924B9C

typedef ClientProceduralTerrainAppearance CPTA;

SET_CLIENT_STATIC(CPTA::ms_staticNonCollidableFloraDistance, GET_NONCOLLIDABLE_FLORA_ADDRESS_PTR);
SET_CLIENT_STATIC(CPTA::ms_dynamicNearFloraDistance, RADIAL_FLORA_ADDRESS);
