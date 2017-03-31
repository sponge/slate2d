#include "scene_gamemap.h"
#include "pecs.h"
#include "../src/sweep.h"
#include "game.h"
#include "components.h"
#include "componenthelpers.h"
#include "lua_extstate.h"

void GameMapScene::Startup(ClientInfo* info) {
	inf = info;

	world = new BaseWorld();

	lua.LoadGameFile("scripts/main.lua");
}

void GameMapScene::Update(float dt) {
	world->update(dt);
}

void GameMapScene::Render() {
	world->render(0);
}

GameMapScene::~GameMapScene() {
	delete world;
}