#pragma once

#include "Object.h"
#include "soewrappers.h"

class CellProperty : public Object {
	DEFINE_CLIENT_STATIC(CellProperty*, ms_worldCellProperty);

public:
	CellProperty() {

	}

	static void setPortalTransitionsEnabled(bool val) {
		runStatic<0x00B2A990, void>(val);
	}

	static CellProperty* getWorldCellProperty() {
		//return runStatic<0x00B2A930, CellProperty*>();
		return ms_worldCellProperty;
	}
};
