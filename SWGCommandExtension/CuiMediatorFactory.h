#pragma once

#include "soewrappers.h"
#include "Object.h"

#define CUIFACTORY_ACTIVATE_ADDRESS 0x008840D0
#define CUIFACTORY_GET_ADDRESS 0x00883FB0

class CuiMediatorFactory {
public:
	static Object* activate(const char* name, const char* unk = nullptr, bool create = true) {
		return Call<CUIFACTORY_ACTIVATE_ADDRESS, Object*, const char*, const char*, bool>::run(name, unk, create);
	}

	static Object* get(const char* name, bool create = true) {
		return Call<CUIFACTORY_GET_ADDRESS, Object*, const char*, bool>::run(name, create);
	}
};