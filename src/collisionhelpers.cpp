#include "local.h"
#include "components.h"

const Sweep Move(ex::EntityManager * es, ex::Entity ent, double dx, double dy, ex::Entity &hitEnt)
{
	auto body = ent.component<Body>();
	auto box = Box(body->pos.x, body->pos.y, body->size.x, body->size.y);

	auto broad = getBroadPhaseBox(box, Vec2(dx, dy));

	Sweep sweep;
	sweep.time = 1.0;
	sweep.pos.x = body->pos.x + dx;
	sweep.pos.y = body->pos.y + dy;

	for (auto ent2 : es->entities_with_components<Body>()) {
		if (ent == ent2) {
			continue;
		}

		auto body2 = ent2.component<Body>();
		auto box2 = Box(body2->pos.x, body2->pos.y, body2->size.x, body2->size.y);

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