#include "systems.h"

const double WORLD_WIDTH = 800.0;
const double WORLD_HEIGHT = 600.0;

void RectMoverSystem::update(EntityManager &es, double dt) {
	for (auto ent : es.entities_with_components<Body, Movable>()) {
		auto body = ent.component<Body>();
		auto movable = ent.component<Movable>();

		double dx = movable->dx * dt;
		double dy = movable->dy * dt;

		auto res = Move(&es, ent, dx, 0);

		if (res.time < 1.0) {
			body->x = res.x;
			body->y = res.y;

			auto dotprod = (dx * res.normalY + dy * res.normalX) * res.timeRemaining;
			dx = dotprod * res.normalY;
			dy = dotprod * res.normalX;

			res = Move(&es, ent, dx, dy);
		}

		body->x = res.x;
		body->y = res.y;

		if (body->x + body->w > WORLD_WIDTH || body->x < 0) {
			body->x = body->x < 0 ? 0 : WORLD_WIDTH - body->w;
			movable->dx *= -1;
		}

		if (body->y + body->h > WORLD_HEIGHT || body->y < 0) {
			body->y = body->y < 0 ? 0 : WORLD_HEIGHT - body->h;
			movable->dy *= -1;
		}
	}
}

