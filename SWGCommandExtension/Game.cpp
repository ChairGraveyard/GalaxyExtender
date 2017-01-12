#include "stdafx.h"

#include "Game.h"
#include "GroundScene.h"

#define GAME_SCENE_ADDRESS 0x0190885C

SET_CLIENT_STATIC(Game::ms_scene, GAME_SCENE_ADDRESS);

GroundScene* Game::getGroundScene() {
	Scene* scene = Game::getScene();

	if (!scene)
		return nullptr;

	GroundScene* ground = reinterpret_cast<GroundScene*>(Object::dynamicCast(scene, (PVOID)Scene::RTTI, (PVOID)GroundScene::RTTI));

	return ground;
}