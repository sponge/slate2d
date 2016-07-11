#include "local.h"

const Sweep Move(EntityManager * es, Entity ent, double dx, double dy)
{
	auto body = ent.component<Body>();
	auto box = Box(body->x, body->y, body->w, body->h);

	// FIXME: broadsweep box generation here?

	double minTime = std::numeric_limits<double>::infinity();
	Sweep sweep;

	for (auto ent2 : es->entities_with_components<Body>()) {
		if (ent == ent2) {
			continue;
		}

		auto body2 = ent2.component<Body>();
		auto box2 = Box(body2->x, body2->y, body2->w, body2->h);

		// FIXME: broadsweep here?

		auto tempSweep = sweepAABB(box, box2, Vec2(dx, dy));
		if (tempSweep.time < minTime) {
			minTime = sweep.time;
			sweep = tempSweep;
		}
	}

	return sweep;
}