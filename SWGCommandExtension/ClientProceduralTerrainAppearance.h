#pragma once

#include "soewrappers.h"
#include "Object.h"

class ClientProceduralTerrainAppearance : public BaseHookedObject {
	class LevelOfDetail {
	public:
		
	};

protected:
	DEFINE_CLIENT_STATIC(float, ms_staticNonCollidableFloraDistance);
	DEFINE_CLIENT_STATIC(float, ms_dynamicNearFloraDistance);

public:
	static float getStaticNonCollidableFloraDistance() {
		return ms_staticNonCollidableFloraDistance;
	}

	static void setStaticNonCollidableFloraDistance(float val) {
		ms_staticNonCollidableFloraDistance = val;
	}

	static float getDynamicNearFloraDistance() {
		return ms_dynamicNearFloraDistance;
	}

	static void setDynamicNearFloraDistance(float val) {
		ms_dynamicNearFloraDistance = val;
	}
};
