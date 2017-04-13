#include "game.h"
#include "sweep.h"
#include "pecs.h"
#include "baseworld.h"

using namespace pecs;

const Sweep Trace(BaseWorld &world, entity_t &ent, float dx, float dy, entity_t *hitEnt)
{
	auto body = world.Bodys[ent.id];
	auto box = Box(body.x, body.y, body.w, body.h);
	auto delta = Vec2(dx, dy);

	auto broad = getBroadPhaseBox(box, delta);

	Sweep sweep;
	sweep.time = 1.0;
	sweep.pos.x = body.x + dx;
	sweep.pos.y = body.y + dy;
	
	for (auto &ent2 : world.entities) {
		if ((ent2.mask & COMPONENT_BODY) == false) {
			continue;
		}

		if (ent.id == ent2.id) {
			continue;
		}

		auto body2 = world.Bodys[ent2.id];

		if (body2.w == 0 || body2.h == 0) {
			continue;
		}

		auto box2 = Box(body2.x, body2.y, body2.w, body2.h);

		if (intersectAABB(broad, box2).valid == false) {
			continue;
		}

		auto tempSweep = sweepAABB(box2, box, delta);
		if (tempSweep.time < sweep.time) {
			sweep = tempSweep;
			hitEnt = &ent2;
		}
	}
	/*
	for (auto ent2 : es.entities_with_components<TileMap>()) {
		auto tmap = ent2.component<TileMap>().get();
		auto tempSweep = Map_SweepTiles(*tmap, box, delta, Vec2(16, 16));
		if (tempSweep.time < sweep.time) {
			sweep = tempSweep;
			hitEnt = ent2;
		}
	}

	es.each<Trigger>([body, box, broad, delta](ex::Entity entity, Trigger &trigger) {
		// FIXME: check broadphase, do sweep, send event if trigger time < sweep.time
	});
	*/

	return sweep;
}