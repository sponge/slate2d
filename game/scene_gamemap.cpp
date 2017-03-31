#include "scene_gamemap.h"
#include "cvar_game.h"
#include "pecs.h"
#include "game.h"
#include "public.h"
#include "lua_extstate.h"

void GameMapScene::Startup(ClientInfo* info) {
	inf = info;

	map = trap->Map_Load(mapFileName);

	trap->Img_LoadAll(inf->nvg);

	// spawn entities

	// allocate and generate tileinfos

	world = new BaseWorld();
	// create all systems

	lua.LoadGameFile("scripts/main.lua");
}

void GameMapScene::Update(float dt) {
	world->update(dt);
}

void GameMapScene::Render() {
	world->render(0);

	if (dbg_drawBbox->integer) {
		// draw debug bounding boxes
	}
}

GameMapScene::~GameMapScene() {
	delete world;
	trap->Map_Free(map);
}