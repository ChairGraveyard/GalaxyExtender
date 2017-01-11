#pragma once

#include "soewrappers.h"
#include "CreatureObject.h"
#include "PlayerObject.h"

#define GAME_GETPLAYER_ADDRESS 0x425140
#define GAME_GETPLAYERCREATURE_ADDRESS 0x425200
#define GAME_GETPLAYEROBJECT_ADDRESS 0x425180

class Game {
public:
	static Object* getPlayer() {
		return Call<GAME_GETPLAYER_ADDRESS, Object*>::run();
	}

	static CreatureObject* getPlayerCreature() {
		return Call<GAME_GETPLAYERCREATURE_ADDRESS, CreatureObject*>::run();
	}

	static PlayerObject* getPlayerObject() {
		return Call<GAME_GETPLAYEROBJECT_ADDRESS, PlayerObject*>::run();
	}
};