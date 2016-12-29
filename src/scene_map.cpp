#include "scene_map.h"
#include <nanovg.h>
#include <tmx.h>
#include "sweep.h"
#include "local.h"

#include "sys/systems_drw.h"
#include "sys/systems_upd.h"
#include "components.h"

// used so the tmx nvg img funcs work. 
NVGcontext *nvg;

void* nvg_img_load_func(const char *path) {
	void *buffer;
	auto sz = FS_ReadFile(path, &buffer);

	Img *img = new Img();
	img->nvg = nvg;
	img->hnd = nvgCreateImageMem(nvg, NVG_IMAGE_NEAREST, (unsigned char *) buffer, sz);
	nvgImageSize(img->nvg, img->hnd, &img->w, &img->h);
	strncpy(img->path, path, sizeof(img->path));

	free(buffer);

	return (void*)img;
}

void nvg_img_free_func(void *address) {
	Img *img = (Img*)address;
	nvgDeleteImage(nvg, img->hnd);
	delete img;
}

void* physfs_file_read_func(const char *path, int *outSz) {
	void *xml;

	*outSz = FS_ReadFile(path, &xml);

	if (outSz < 0) {
		Com_Error(ERR_DROP, "Couldn't load file while parsing map %s", path);
	}

	return (void *)xml;
}

GameWorld::GameWorld(const char *filename) {
	systems.add<InputSystem>();
	systems.add<PlayerSystem>();
	systems.add<CameraUpdateSystem>();
	systems.configure();

	tmx_img_load_func = nvg_img_load_func;
	tmx_img_free_func = nvg_img_free_func;
	tmx_file_read_func = physfs_file_read_func;

	auto map = tmx_load(filename);

	if (!map) {
		Com_Error(ERR_DROP, "Failed to load map");
	}

	auto world = this->entities.create();
	auto tileMap = world.assign<TileMap>();
	tileMap->map = map;
	tmap = world.component<TileMap>().get();

	tmx_layer *layer = map->ly_head;
	while (layer) {
		if (layer->visible == false) {
			layer = layer->next;
			continue;
		}

		if (strcmp(layer->name, "world") == 0) {
			tileMap->worldLayer = layer;
		}

		if (layer->type == L_OBJGR) {
			// parse objects
		}

		layer = layer->next;
	}

	auto ent = this->entities.create();
	auto boxBody = ent.assign<Body>(306, 178, 14, 28);
	ent.assign<Movable>(0, 0);
	ent.assign<Renderable>(200, 30, 30, 255);
	ent.assign<PlayerInput>();

	auto camera = world.assign<Camera>(1280, 720, 3, map->width * map->tile_width, map->height * map->tile_height);
	camera->target = boxBody.get();
}

void GameWorld::update(ex::TimeDelta dt) {
	systems.update<InputSystem>(dt);
	systems.update<PlayerSystem>(dt);
	systems.update<CameraUpdateSystem>(dt);
}

void MapScene::Startup(ClientInfo* info) {
	inf = info;
	nvg = info->nvg;

	world = new GameWorld(fileName);

	rendSys = new ex::SystemManager(world->entities, world->events);
	rendSys->add<CameraDrawSystem>(inf);
	rendSys->add<TileMapDrawSystem>(inf);
	rendSys->add<RectDrawSystem>(inf);
	rendSys->configure();
}

GameWorld::~GameWorld() {
	tmx_map_free(tmap->map);
}

void MapScene::Update(double dt) {
	world->update(dt);
}

MapScene::MapScene(const char *fileName) {
	this->fileName = fileName;
}

void MapScene::Render() {
	rendSys->update<CameraDrawSystem>(0);
	rendSys->update<TileMapDrawSystem>(0);
	rendSys->update<RectDrawSystem>(0);
}

MapScene::~MapScene() {
	delete world;
	delete rendSys;
}