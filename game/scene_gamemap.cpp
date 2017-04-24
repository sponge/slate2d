#include "scene_gamemap.h"
#include "cvar_game.h"
#include "pecs.h"
#include "game.h"
#include "public.h"
#include "lua_extstate.h"
#include "systems/systems.h"

void GameMapScene::Startup(ClientInfo* info) {
	inf = info;

	// load the map and images
	map = trap->Map_Load(mapFileName);

	world = new BaseWorld();

	// update systems (most are in lua)
	world->add(new PlayerInputSystem());

	// render systems
	world->add(new CameraDrawSystem(info));
	world->add(new TileMapDrawSystem(info));
	world->add(new SpriteDrawerSystem(info));
	world->add(new RectDrawerSystem(info));

	// our "master" entity
	entity_t worldEnt = world->get_entity();
	auto tmap = TileMap();
	tmap.map = map;

	// look for a layer named "world" and error out if we can't find one as it's required
	tmx_layer *layer = map->ly_head;
	while (layer) {
		if (strcmp(layer->name, "world") == 0 && layer->visible == true) {
			tmap.worldLayer = layer;
			break;
		}

		layer = layer->next;
	}

	if (tmap.worldLayer == nullptr) {
		trap->Error(ERR_DROP, "No visible layer named \"world\" found in map.");
		return;
	}

	// generate tileinfo structs so we know properties about
	tmap.tinfo = (TileInfo *)malloc(sizeof(TileInfo) * map->tilecount);
	this->tinfo = tmap.tinfo;

	for (unsigned int i = 1; i < map->tilecount; i++) {
		auto tile = map->tiles[i];
		if (tile == nullptr) {
			break;
		}
		auto &info = tmap.tinfo[i];
		info.gid = i;
		info.solid = true;
		info.platform = false;

		auto prop = tile->properties;
		while (prop != nullptr) {
			if (strcasecmp(prop->name, "solid") == 0) {
				info.solid = atoi(prop->value) > 0;
			}
			else if (strcasecmp(prop->name, "platform") == 0) {
				info.platform = atoi(prop->value) > 0;
			}
			prop = prop->next;
		}

	}

	// assign the world entity now (since it will do a copy)
	world->assign(&worldEnt, tmap);
	world->add(worldEnt);
	world->masterEntity = worldEnt.id;

	// load the lua script and check for a spawn_entity global func
	lua.LoadGameFile("scripts/main.lua");

	sol::function initFunc = lua["init"];
	if (!initFunc.valid()) {
		trap->Error(ERR_DROP, "couldn't find function init in main.lua");
		return;
	}
	initFunc();

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

		const tmx_object *obj = layer->content.objgr->head;
		while (obj != nullptr) {
			if (obj->type == nullptr) {
				obj = obj->next;
				continue;
			}

			auto props = lua.create_table_with();
			const tmx_property *prop = obj->properties;
			while (prop != nullptr) {
				props[prop->name] = prop->value;
				prop = prop->next;
			}

			spawnFunc(world, obj, props);
			obj = obj->next;
		}

		layer = layer->next;
	}

	trap->Img_LoadAll();
}

void GameMapScene::Update(float dt) {
	world->time += dt;
	world->update(dt);
}

void GameMapScene::Render() {
	world->render(0);
}

GameMapScene::~GameMapScene() {
	for (auto sys : world->systems) {
		delete sys;
	}
	delete world;
	trap->Map_Free(map);

	if (this->tinfo != nullptr) {
		free(this->tinfo);
	}
}