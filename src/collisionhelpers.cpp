#include "local.h"

const Sweep Move(EntityManager * es, Entity ent, double dx, double dy, Entity &hitEnt)
{
	auto body = ent.component<Body>();
	auto box = Box(body->x, body->y, body->w, body->h);

	auto broad = getBroadPhaseBox(box, Vec2(dx, dy));

	Sweep sweep;
	sweep.time = 1.0;
	sweep.pos.x = body->x + dx;
	sweep.pos.y = body->y + dy;

	for (auto ent2 : es->entities_with_components<Body>()) {
		if (ent == ent2) {
			continue;
		}

		auto body2 = ent2.component<Body>();
		auto box2 = Box(body2->x, body2->y, body2->w, body2->h);

		if (intersectAABB(broad, box2).valid == false) {
			continue;
		}

		auto tempSweep = sweepAABB(box2, box, Vec2(dx, dy));
		if (tempSweep.time < sweep.time) {
			sweep = tempSweep;
			hitEnt = ent2;
		}
	}

	return sweep;
}