#include "scene_map.h"
#include <nanovg.h>
#include <random>
#include "sys/systems.h"
#include "sweep.h"
#include "local.h"

NVGcontext *nvg;
Body *body;

void* nvg_img_load_func(const char *path) {
	Img *img = new Img();
	img->nvg = nvg;
	img->hnd = nvgCreateImage(nvg, path, 0);
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

	// temp
	auto ent = es.create();
	auto boxBody = ent.assign<Body>(0, 0, 14, 28);
	ent.assign<Movable>(30, 20);
	ent.assign<Renderable>(200, 30, 30, 255);
	ent.assign<PlayerInput>();

	auto camera = world.assign<Camera>(0, 0, inf->width, inf->height);
	camera->target = boxBody.get();
	body = boxBody.get();

	updateSystems.push_back(new InputSystem());
	updateSystems.push_back(new PlayerSystem());
	updateSystems.push_back(new CameraUpdateSystem());
	//updateSystems.push_back(new RectMoverSystem());

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
	auto collide = x > 50 || y > 35 || x < 15 || y < 5;
#ifdef DEBUG
	nvgBeginPath(nvg);
	nvgFillColor(nvg, collide ? nvgRGBA(150, 0, 0, 150) : nvgRGBA(0, 150, 0, 150));
	nvgRect(nvg, x * 16, y*16, 16, 16);
	nvgFill(nvg);
#endif
	return collide ? (void*)0x00000001 : nullptr;
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

			// assign a direction vector so we can project outward from the corner toward the 2 corners
			// (used to loop from position to testing box width/height)
#ifdef DEBUG
			// pos of the 2 corners that are min/max bounds of the line
			nonIntersect[0] = cornerArr[(i + 1) % 4];
			nonIntersect[1] = cornerArr[(i + 3) % 4];
#endif
			break;
		};
	}
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
#endif

	// size of the check box in tiles, rounded up.
	auto boxTileSize = Vec2(ceil(check.size.x / tileSize.x), ceil(check.size.y / tileSize.y));

	// tile position of start and end of move
	auto startTile = Vec2(floor(opp.x / tileSize.x), floor(opp.y / tileSize.y));
	auto endTile = Vec2(floor((opp.x + delta.x) / tileSize.x), floor((opp.y + delta.y) / tileSize.y));

	// adapted from http://www.redblobgames.com/grids/line-drawing.html - 2.1 orthagonal steps
	auto d = Vec2(endTile.x - startTile.x, endTile.y - startTile.y);
	auto n = Vec2(abs(d.x), abs(d.y));

	// current point we're stepping through
	auto p = startTile;

	// whether we found a collision in either axis
	auto xCollided = false, yCollided = false;

	// step through the tiles
	for (int ix = 0, iy = 0; (ix < n.x || iy < n.y);) {
		// break if we've found collisions in both axises
		if (xCollided == true && yCollided == true) {
			break;
		}

		if ((0.5 + ix) / n.x < (0.5 + iy) / n.y) {
			// step is in the x direction
			p.x += sign(d.x);

			if (xCollided == true) {
				ix++;
				continue;
			}

			// start from the outside corner and move toward the center line looking for a hit
			for (int i = 0; i <= boxTileSize.y; i++) {
				auto t = getTile(p.x, p.y + i * direction.y);
				if (t == nullptr) {
					continue;
				}
				xCollided = isResolvable(t);
				if (xCollided) {
#ifdef DEBUG
					// draw a box around a confirmed hit
					nvgBeginPath(nvg);
					nvgStrokeColor(nvg, nvgRGBA(255, 255, 0, 255));
					nvgStrokeWidth(nvg, 1);
					nvgRect(nvg, p.x * tileSize.x, (p.y + i * direction.y) * tileSize.y, tileSize.x, tileSize.y);
					nvgStroke(nvg);
#endif
					// we found a collision on x, calculate the time of the collision
					auto box = Box(p.x * tileSize.x + tileSize.x / 2, (p.y + i * direction.y) * tileSize.y + tileSize.y / 2, tileSize.x, tileSize.y);
					auto tempSweep = sweepAABB(box, check, delta);
					sweep = tempSweep.time < sweep.time ? tempSweep : sweep;
				}

			}

			// we don't need to go past the current column so if we've found the other collision we can stop now
			if (yCollided) {
				xCollided = true;
			}

			ix++;
		}
		else {
			// do the same thing for the y axis
			p.y += sign(d.y);
			if (yCollided == true) {
				iy++;
				continue;
			}

			for (int i = 0; i <= boxTileSize.x; i++) {
				auto t = getTile(p.x + i * direction.x, p.y);
				if (t == nullptr) {
					continue;
				}
				yCollided = isResolvable(t);
				if (yCollided) {
#ifdef DEBUG
					// draw a box around a confirmed hit
					nvgBeginPath(nvg);
					nvgStrokeColor(nvg, nvgRGBA(0, 255, 255, 255));
					nvgStrokeWidth(nvg, 1);
					nvgRect(nvg, (p.x + i * direction.x) * tileSize.x, p.y * tileSize.y, tileSize.x, tileSize.y);
					nvgStroke(nvg);
#endif
					auto box = Box((p.x + i * direction.x) * tileSize.x + tileSize.x / 2, p.y * tileSize.y + tileSize.y / 2, tileSize.x, tileSize.y);
					auto tempSweep = sweepAABB(box, check, delta);
					sweep = tempSweep.time < sweep.time ? tempSweep : sweep;
				}
			}

			// we don't need to go past the current column so if we've found the other collision we can stop now
			if (xCollided) {
				yCollided = true;
			}

			iy++;
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

	// box
	nvgBeginPath(nvg);
	nvgStrokeColor(nvg, nvgRGBA(255, 0, 0, 255));
	nvgStrokeWidth(nvg, 1);
	nvgRect(nvg, body->pos.x - body->half.x, body->pos.y - body->half.y, body->size.x, body->size.y);
	nvgStroke(nvg);

	auto sweep = _sweepTiles(*body, delta, tileSize, &getTile, &isResolvable);

	// destination box
	nvgBeginPath(nvg);
	nvgStrokeColor(nvg, nvgRGBA(0, 255, 0, 255));
	nvgStrokeWidth(nvg, 1);
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