#include "scene_gamemap.h"
#include "cvar_game.h"
#include "pecs.h"
#include "game.h"
#include "public.h"
#include "lua_extstate.h"

void GameMapScene::Startup(ClientInfo* info) {
	inf = info;

	// load the map and images
	map = trap->Map_Load(mapFileName);

	trap->Img_LoadAll(inf->nvg);

	world = new BaseWorld();

	// our "master" entity
	entity_t worldEnt = world->get_entity();
	auto tmap = TileMap();
	tmap.map = map;

	// look for a layer named "world" and error out if we can't find one as it's required
	tmx_layer *layer = map->ly_head;
	while (layer) {
		if (layer->visible == false) {
			layer = layer->next;
			continue;
		}

		if (strcmp(layer->name, "world") == 0) {
			tmap.worldLayer = layer;
			break;
		}

		layer = layer->next;
	}

	if (tmap.worldLayer == nullptr) {
		trap->Error(ERR_DROP, "No visible layer named \"world\" found in map.");
	}

	// FIXME: allocate and generate tileinfos

	world->assign(&worldEnt, tmap);

	// load the lua script and check for a spawn_entity global func
	lua.LoadGameFile("scripts/main.lua");

	sol::function spawnFunc = lua["spawn_entity"];
	if (!spawnFunc.valid()) {
		trap->Error(ERR_DROP, "couldn't find function spawn_entity in main.lua");
		return;
	}

	// loop through layers again looking for object groups, and send them out to lua for spawning logic
	layer = map->ly_head;
	while (layer) {
		if (layer->visible == false || layer->type != L_OBJGR) {
			layer = layer->next;
			continue;
		}

		tmx_object *obj = layer->content.objgr->head;
		while (obj != nullptr) {
			if (obj->type == nullptr) {
				obj = obj->next;
				continue;
			}

			auto ent = lua.create_table_with(
				"id", obj->id,
				"x", obj->x,
				"y", obj->y,
				"width", obj->width,
				"height", obj->height,
				"gid", obj->gid,
				"visible", obj->visible,
				"rotation", obj->rotation,
				"name", obj->name,
				"type", obj->type
			);

			ent["properties"] = lua.create_table_with();
			tmx_property *prop = obj->properties;
			while (prop != nullptr) {
				ent["properties"][prop->name] = prop->value;
				prop = prop->next;
			}

			spawnFunc(world, ent);
			obj = obj->next;
		}

		layer = layer->next;
	}
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