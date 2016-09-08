#include "scene_map.h"
#include <nanovg.h>
#include <random>
#include "sys/systems.h"
#include "sweep.h"
#include "local.h"

NVGcontext *nvg;
Body *body;
tmx_map *tmap;
tmx_layer *wlayer;

void* nvg_img_load_func(const char *path) {
	Img *img = new Img();
	img->nvg = nvg;
	img->hnd = nvgCreateImage(nvg, path, NVG_IMAGE_NEAREST);
	nvgImageSize(img->nvg, img->hnd, &img->w, &img->h);
	strncpy(img->path, path, sizeof(img->path));
	return (void*)img;
}

void nvg_img_free_func(void *address) {
	Img *img = (Img*)address;
	nvgDeleteImage(nvg, img->hnd);
	delete(img);
}

void MapScene::Startup(ClientInfo* info) {
	inf = info;
	nvg = info->nvg;

	tmx_img_load_func = nvg_img_load_func;
	tmx_img_free_func = nvg_img_free_func;

	auto map = tmx_load("base/maps/smw.tmx");
	if (!map) tmx_perror("error");

	Entity world = es.create();
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

	// temp
	auto ent = es.create();
	auto boxBody = ent.assign<Body>(306, 178, 14, 28);
	ent.assign<Movable>(0, 0);
	ent.assign<Renderable>(200, 30, 30, 255);
	ent.assign<PlayerInput>();

	auto camera = world.assign<Camera>(0, 0, inf->width, inf->height);
	camera->target = boxBody.get();
	body = boxBody.get();

	updateSystems.push_back(new InputSystem());
	updateSystems.push_back(new PlayerSystem());
	updateSystems.push_back(new CameraUpdateSystem());

	renderSystems.push_back(new CameraDrawSystem());
	renderSystems.push_back(new TileMapDrawSystem());
	renderSystems.push_back(new RectDrawSystem());
}

void MapScene::Update(double dt) {
	for (auto sys : updateSystems) {
		sys->update(es, dt);
	}
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

Sweep _sweepTiles(Box check, Vec2 delta, Vec2 tileSize, void *(*getTile)(int x, int y), bool(*isResolvable)(void *tile)) {
	auto sweep = Sweep();
	sweep.pos.x = check.pos.x + delta.x;
	sweep.pos.y = check.pos.y + delta.y;

	if (delta.x == 0 && delta.y == 0) {
		sweep.time = 0;
		return sweep;
	}

	auto corners = check.corners();
	Vec2 cornerArr[4] = { corners.tl, corners.tr, corners.br, corners.bl };
	// the inside corner that we're tracing on
	Vec2 opp;
	// the direction we're going to project out and over to cover the width/height of the box
	Vec2 direction;
#ifdef DEBUG
	// the outside corners used for debug drawing
	Vec2 nonIntersect[2];
#endif
	// find the corner that's occluded by the box by checking for a collision
	if (delta.y == 0) {
		opp = delta.x > 0 ? corners.br : corners.bl;
	}
	else if (delta.x == 0) {
		opp = delta.y > 0 ? corners.br : corners.tr;
	}
	else {
		for (int i = 0; i < sizeof(cornerArr); i++) {
			auto corner = cornerArr[i];
			auto hit = intersectSegment(check, corner, delta);
			if (hit.valid == false) {
				continue;
			}

			// get the opposite corner of the corner that intersected itself, this is
			// the deepest point inside the sweep
			opp = cornerArr[(i + 2) % 4];

#ifdef DEBUG
			// pos of the 2 corners that are min/max bounds of the line
			nonIntersect[0] = cornerArr[(i + 1) % 4];
			nonIntersect[1] = cornerArr[(i + 3) % 4];
#endif
			break;
		};
	}

	// assign a direction vector so we can project outward from the corner toward the 2 corners
	// (used to loop from position to testing box width/height)
	direction = Vec2(0 - sign(delta.x), 0 - sign(delta.y));

#ifdef DEBUG
	// debug draw the outside corners of the rect. the background tiles should always fill the lines
	for (auto corner : nonIntersect) {
		nvgStrokeColor(nvg, nvgRGBA(0, 255, 0, 50));
		nvgBeginPath(nvg);
		nvgMoveTo(nvg, corner.x, corner.y);
		nvgLineTo(nvg, corner.x + delta.x, corner.y + delta.y);
		nvgStroke(nvg);
	}

	nvgStrokeColor(nvg, nvgRGBA(0, 0, 255, 255));
	nvgBeginPath(nvg);
	nvgMoveTo(nvg, opp.x, opp.y);
	nvgLineTo(nvg, opp.x + delta.x, opp.y + delta.y);
	nvgStroke(nvg);
#endif

	// size of the check box in tiles, rounded up.
	// FIXME: this doesn't work, i need to find out how many tiles it goes over at the point of the line
	auto boxTileSize = Vec2(ceil(check.size.x / tileSize.x), ceil(check.size.y / tileSize.y));
	//boxTileSize.x = Cvar_Get("size_x", "0", 0)->integer;
	//boxTileSize.y = Cvar_Get("size_y", "0", 0)->integer;

	// http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
	auto x0 = opp.x / 16;
	auto x1 = (opp.x + delta.x) / 16;
	auto y0 = opp.y / 16;
	auto y1 = (opp.y + delta.y) / 16;

	double dx = fabs(x1 - x0);
	double dy = fabs(y1 - y0);

	int x = int(floor(x0));
	int y = int(floor(y0));

	int n = 1;
	int x_inc, y_inc;
	double error;

	if (dx == 0) {
		x_inc = 0;
		error = std::numeric_limits<double>::infinity();
	}
	else if (x1 > x0) {
		x_inc = 1;
		n += int(floor(x1)) - x;
		error = (floor(x0) + 1 - x0) * dy;
	}
	else {
		x_inc = -1;
		n += x - int(floor(x1));
		error = (x0 - floor(x0)) * dy;
	}

	if (dy == 0) {
		y_inc = 0;
		error -= std::numeric_limits<double>::infinity();
	}
	else if (y1 > y0) {
		y_inc = 1;
		n += int(floor(y1)) - y;
		error -= (floor(y0) + 1 - y0) * dx;
	}
	else {
		y_inc = -1;
		n += y - int(floor(y1));
		error -= (y0 - floor(y0)) * dx;
	}

	for (; n > 1; --n) {
		if (error > 0) {
			// step is in the y direction
			y += y_inc;

			// start from the outside corner and move toward the center line looking for a hit
			for (int i = 0; i <= boxTileSize.x; i++) {
				auto lx = x + i * direction.x;
				auto ly = y;
				auto t = getTile(lx, ly);
				if (t == nullptr) {
					continue;
				}
				auto xCollided = isResolvable(t);
				if (xCollided) {
#ifdef DEBUG
					// draw a box around a confirmed hit
					nvgBeginPath(nvg);
					nvgRect(nvg, lx * tileSize.x, ly * tileSize.y, tileSize.x, tileSize.y);
#endif
					// we found a collision on x, calculate the time of the collision
					auto box = Box(lx * tileSize.x + tileSize.x / 2, ly * tileSize.y + tileSize.y / 2, tileSize.x, tileSize.y);
					sweep = sweepAABB(box, check, delta);
					if (sweep.time < 1.0) {
						return sweep;
					}
				}
			}

			error -= dx;
		}
		else {
			// step is in the x direction
			x += x_inc;

			for (int i = 0; i <= boxTileSize.y; i++) {
				auto lx = x;
				auto ly = y + i * direction.y;
				auto t = getTile(lx, ly);
				if (t == nullptr) {
					continue;
				}
				auto yCollided = isResolvable(t);
				if (yCollided) {
#ifdef DEBUG
					// draw a box around a confirmed hit
					nvgBeginPath(nvg);
					nvgRect(nvg, lx * tileSize.x, ly * tileSize.y, tileSize.x, tileSize.y);
#endif
					// we found a collision on x, calculate the time of the collision
					auto box = Box(lx * tileSize.x + tileSize.x / 2, ly * tileSize.y + tileSize.y / 2, tileSize.x, tileSize.y);
					sweep = sweepAABB(box, check, delta);
					if (sweep.time < 1.0) {
						return sweep;
					}
				}
			}

			error += dy;
		}
	}

	return sweep;
}

void MapScene::Render() {
	for (auto sys : renderSystems) {
		sys->render(es, inf);
	}
	nvg = inf->nvg;

	int x, y;
	SDL_GetMouseState(&x, &y);
	Vec2 delta = Vec2(x - body->pos.x, y - body->pos.y);
	Vec2 tileSize = Vec2(16, 16);

	auto sweep = _sweepTiles(*body, delta, tileSize, &getTile, &isResolvable);

	// destination box
	nvgBeginPath(nvg);
	nvgStrokeColor(nvg, nvgRGBA(0, 255, 0, 255));
	nvgStrokeWidth(nvg, 0.5);
	nvgRect(nvg, sweep.pos.x - body->half.x, sweep.pos.y - body->half.y, body->size.x, body->size.y);
	nvgStroke(nvg);
}

void MapScene::Teardown() {
	for (auto sys : updateSystems) {
		delete(sys);
	}
	updateSystems.clear();

	for (auto sys : renderSystems) {
		delete(sys);
	
	}
	renderSystems.clear();

	// FIXME: free the map
	//tmx_map_free(map);

	tmx_img_load_func = nullptr;
	tmx_img_free_func = nullptr;
}