#include <entityx/entityx.h>
#include "components.h"

namespace ex = entityx;

bool Map_IsTileResolvable(TileMap &map, Box check, unsigned int tx, unsigned int ty, Vec2 delta) {
	bool resolved = false;
	unsigned int gid = (tx >= map.map->width || ty >= map.map->height) ? 1 : (map.worldLayer->content.gids[(ty*map.map->width) + tx]) & TMX_FLIP_BITS_REMOVAL;

	if (gid == 0) {
		return false;
	}

	// TODO: if last tile, check slope?
	if (map.tinfo[gid].platform) {
		auto bottom = check.max().y;
		resolved = delta.y > 0 && bottom <= ty * map.map->tile_height && bottom + delta.y > ty * map.map->tile_height;
	}
	else {
		resolved = map.tinfo[gid].solid;
	}

	return resolved;
}

// WARNING: this might not be safe for large deltas anymore, but should if the two callbacks are factored out
// FIXME: make this pass in std::function for resolvable tile again since i found out about bound
Sweep Map_SweepTiles(TileMap &map, Box check, Vec2 delta, Vec2 tileSize) {
	auto sweep = Sweep();
	sweep.pos.x = check.pos.x + delta.x;
	sweep.pos.y = check.pos.y + delta.y;

	if (delta.x == 0 && delta.y == 0) {
		sweep.time = 0;
		return sweep;
	}

	auto corners = check.corners();
	Vec2 opp;
	if (sign(delta.x) >= 0) {
		opp = sign(delta.y) >= 0 ? corners.br : corners.tr;
	} else {
		opp = sign(delta.y) >= 0 ? corners.bl : corners.tl;
	}

	// assign a direction vector so we can project outward from the corner toward the 2 corners
	// (used to loop from position to testing box width/height)
	Vec2 direction = Vec2(0 - sign(delta.x), 0 - sign(delta.y));

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

	float dx = fabs(x1 - x0);
	float dy = fabs(y1 - y0);

	int x = int(floor(x0));
	int y = int(floor(y0));

	int n = 1;
	int x_inc, y_inc;
	float error;

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
				if (Map_IsTileResolvable(map, check, lx, ly, delta)) {
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
				if (Map_IsTileResolvable(map, check, lx, ly, delta)) {
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

const Sweep Trace(ex::EntityManager &es, ex::Entity ent, float dx, float dy, ex::Entity &hitEnt)
{
	auto body = ent.component<Body>();
	auto box = Box(body->pos.x, body->pos.y, body->size.x, body->size.y);
	auto delta = Vec2(dx, dy);

	auto broad = getBroadPhaseBox(box, delta);

	Sweep sweep;
	sweep.time = 1.0;
	sweep.pos.x = body->pos.x + dx;
	sweep.pos.y = body->pos.y + dy;

	for (auto ent2 : es.entities_with_components<Body>()) {
		if (ent == ent2) {
			continue;
		}

		auto body2 = ent2.component<Body>();

		if (body2->size.x == 0 || body2->size.y == 0) {
			continue;
		}

		auto box2 = Box(body2->pos.x, body2->pos.y, body2->size.x, body2->size.y);

		if (intersectAABB(broad, box2).valid == false) {
			continue;
		}

		auto tempSweep = sweepAABB(box2, box, delta);
		if (tempSweep.time < sweep.time) {
			sweep = tempSweep;
			hitEnt = ent2;
		}
	}

	for (auto ent2 : es.entities_with_components<TileMap>()) {
		auto tmap = ent2.component<TileMap>().get();
		auto tempSweep = Map_SweepTiles(*tmap, box, delta, Vec2(16,16));
		if (tempSweep.time < sweep.time) {
			sweep = tempSweep;
			hitEnt = ent2;
		}
	}

	es.each<Trigger>([body, box, broad, delta](ex::Entity entity, Trigger &trigger) {
		// FIXME: check broadphase, do sweep, send event if trigger time < sweep.time
	});
	

	return sweep;
}

void Cam_Center(Camera &cam, float cx, float cy) {
	Cam_Move(cam, cx - (cam.size.x / 2) / cam.scale, cy - (cam.size.y / 2) / cam.scale);
}

void Cam_Move(Camera &cam, float x, float y) {
	cam.pos.x = x;
	cam.pos.y = y;
	cam.top = y;
	cam.right = x + (cam.size.x / cam.scale);
	cam.bottom = y + (cam.size.y / cam.scale);
	cam.left = x;
}

void Cam_Bind(Camera &cam) {
	float x = cam.pos.x, y = cam.pos.y;

	if (cam.left < 0) {
		x = 0;
	}

	if (cam.right > cam.max.x) {
		x = cam.max.x - (cam.size.x / cam.scale);
	}

	if (cam.top < 0) {
		y = 0;
	}

	if (cam.bottom > cam.max.y) {
		y = cam.max.y - (cam.size.y / cam.scale);
	}

	Cam_Move(cam, x, y);
}