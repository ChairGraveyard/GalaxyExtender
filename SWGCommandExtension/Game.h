#pragma once

#include "soewrappers.h"
#include "CreatureObject.h"
#include "PlayerObject.h"
#include "GroundScene.h"

#define GAME_GETPLAYER_ADDRESS 0x425140
#define GAME_GETPLAYERCREATURE_ADDRESS 0x425200
#define GAME_GETPLAYEROBJECT_ADDRESS 0x425180
#define ECHO_ADDRESS 0x424810

class Game {
protected:
	DEFINE_CLIENT_STATIC(Scene*, ms_scene);
public:
	static Scene* getScene() {
		return ms_scene;
	}

	static GroundScene* getGroundScene();

	static Object* getPlayer() {
		return Call<GAME_GETPLAYER_ADDRESS, Object*>::run();
	}

	static CreatureObject* getPlayerCreature() {
		return Call<GAME_GETPLAYERCREATURE_ADDRESS, CreatureObject*>::run();
	}

	static PlayerObject* getPlayerObject() {
		return Call<GAME_GETPLAYEROBJECT_ADDRESS, PlayerObject*>::run();
	}

	static void debugPrintUi(const char* str) {
		Call<ECHO_ADDRESS, void, const char*>::run(str);
	}
};