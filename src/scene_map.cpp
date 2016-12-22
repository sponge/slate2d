#include "scene_map.h"
#include <nanovg.h>
#include <tmx.h>
#include "sweep.h"
#include "local.h"

#include "sys/systems_drw.h"
#include "sys/systems_upd.h"
#include "components.h"

NVGcontext *nvg;
Body *body;
tmx_map *tmap;
tmx_layer *wlayer;

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
		Com_Error(ERR_FATAL, "Couldn't load file while parsing map %s", path);
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
		Com_Error(ERR_FATAL, "Map loading failed");
	}

	auto world = this->entities.create();
	auto tileMap = world.assign<TileMap>();
	tileMap->map = map;
	tmap = map;

	tmx_layer *layer = map->ly_head;
	while (layer) {
		if (layer->visible == false) {
			layer = layer->next;
			continue;
		}

		if (strcmp(layer->name, "world") == 0) {
			tileMap->worldLayer = layer;
			wlayer = layer;
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

	auto camera = world.assign<Camera>(0, 0, 1280, 720);
	camera->target = boxBody.get();
	body = boxBody.get();
}

void GameWorld::update(ex::TimeDelta dt) {
	systems.update_all(dt);
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

void MapScene::Update(double dt) {
	world->update(dt);
}

void* getTile(int x, int y) {
	auto gid = (x < 0 || y < 0) ? 0 : (wlayer->content.gids[(y*tmap->width) + x]) & TMX_FLIP_BITS_REMOVAL;
#ifdef DEBUG
	nvgBeginPath(nvg);
	nvgFillColor(nvg, gid > 0 ? nvgRGBA(150, 0, 0, 150) : nvgRGBA(0, 150, 0, 150));
	nvgRect(nvg, x * 16, y*16, 16, 16);
	nvgFill(nvg);
	nvgFontSize(nvg, 4);
	nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255));
	nvgText(nvg, x * 16 + 8, y * 16 + 8, va("%i, %i", x, y), nullptr);
#endif
	return (void*) gid;
}

bool isResolvable(void *tile) {
	return tile != nullptr;
}

MapScene::MapScene(const char *fileName) {
	this->fileName = fileName;
}

void MapScene::Render() {
	rendSys->update<CameraDrawSystem>(0);
	rendSys->update<TileMapDrawSystem>(0);
	rendSys->update<RectDrawSystem>(0);

	nvg = inf->nvg;

	int x, y;
	SDL_GetMouseState(&x, &y);
	Vec2 delta = Vec2(x - body->pos.x, y - body->pos.y);
	Vec2 tileSize = Vec2(16, 16);

	auto sweep = sweepTiles(*body, delta, tileSize, &getTile, &isResolvable);

	// destination box
	nvgBeginPath(nvg);
	nvgStrokeColor(nvg, nvgRGBA(0, 255, 0, 255));
	nvgStrokeWidth(nvg, 0.5);
	nvgRect(nvg, sweep.pos.x - body->half.x, sweep.pos.y - body->half.y, body->size.x, body->size.y);
	nvgStroke(nvg);
}

MapScene::~MapScene() {
	delete world;
	delete rendSys;

	// FIXME: free the map
	//tmx_map_free(map);
}