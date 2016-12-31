#include "scene_map.h"
#include <nanovg.h>
#include <tmx.h>
#include "sweep.h"
#include "local.h"

#include "sys/systems_drw.h"
#include "sys/systems_upd.h"
#include "components.h"

GameWorld::GameWorld(const char *filename) {
	systems.add<InputSystem>();
	systems.add<PlayerSystem>();
	systems.add<CameraUpdateSystem>();
	systems.configure();

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
			tmx_object *obj = layer->content.objgr->head;
			while (obj != nullptr) {
				if (obj->type == nullptr) {
					obj = obj->next;
					continue;
				}

				if (strcmp("player", obj->type) == 0) {
					auto ent = this->entities.create();
					auto body = ent.assign<Body>(obj->x + (map->tile_width / 2), obj->y - 14.001, 14, 28);  // FIXME able to fall into ground if 14
					ent.assign<Movable>(0, 0);
					ent.assign<Renderable>(200, 30, 30, 255);
					ent.assign<PlayerInput>();

					auto camera = world.assign<Camera>(1280, 720, 3, map->width * map->tile_width, map->height * map->tile_height);
					camera->target = body.get();
				}
				obj = obj->next;
			}
			// parse objects
		}

		layer = layer->next;
	}
}

GameWorld::~GameWorld() {
	tmx_map_free(tmap->map);
}

void GameWorld::update(ex::TimeDelta dt) {
	systems.update<InputSystem>(dt);
	systems.update<PlayerSystem>(dt);
	systems.update<CameraUpdateSystem>(dt);
}

// --------------------

MapScene::MapScene(const char *fileName) {
	this->fileName = fileName;
}

MapScene::~MapScene() {
	delete world;
	delete rendSys;
}

void MapScene::Startup(ClientInfo* info) {
	using namespace std::placeholders; // for `_1`

	inf = info;

	tmx_img_load_func = std::bind(&MapScene::nvg_img_load_func, this, _1);
	tmx_img_free_func = std::bind(&MapScene::nvg_img_free_func, this, _1);
	tmx_file_read_func = std::bind(&MapScene::physfs_file_read_func, this, _1, _2);;

	world = new GameWorld(fileName);

	rendSys = new ex::SystemManager(world->entities, world->events);
	rendSys->add<CameraDrawSystem>(inf);
	rendSys->add<TileMapDrawSystem>(inf);
	rendSys->add<RectDrawSystem>(inf);
	rendSys->configure();
}

void MapScene::Update(double dt) {
	world->update(dt);
}

void MapScene::Render() {
	rendSys->update<CameraDrawSystem>(0);
	rendSys->update<TileMapDrawSystem>(0);
	rendSys->update<RectDrawSystem>(0);
}

void* MapScene::nvg_img_load_func(const char *path) {
	void *buffer;
	auto sz = FS_ReadFile(path, &buffer);

	Img *img = new Img();
	img->nvg = inf->nvg;
	img->hnd = nvgCreateImageMem(img->nvg, NVG_IMAGE_NEAREST, (unsigned char *) buffer, sz);
	nvgImageSize(img->nvg, img->hnd, &img->w, &img->h);
	strncpy(img->path, path, sizeof(img->path));

	free(buffer);

	return (void*)img;
}

void MapScene::nvg_img_free_func(void *address) {
	Img *img = (Img*)address;
	nvgDeleteImage(img->nvg, img->hnd);
	delete img;
}

void* MapScene::physfs_file_read_func(const char *path, int *outSz) {
	void *xml;

	*outSz = FS_ReadFile(path, &xml);

	if (outSz < 0) {
		Com_Error(ERR_DROP, "Couldn't load file while parsing map %s", path);
	}

	return (void *)xml;
}