#include "systems.h"

const double WORLD_WIDTH = 800.0;
const double WORLD_HEIGHT = 600.0;

void RectMoverSystem::update(EntityManager &es, double dt) {
	if (dt == 0) {
		return;
	}
	dt = dt > 0.1 ? 0.1 : dt;
	for (auto ent : es.entities_with_components<Body, Movable>()) {
		auto body = ent.component<Body>();
		auto movable = ent.component<Movable>();

		double dx = movable->dx * dt;
		double dy = movable->dy * dt;

		auto move = Move(&es, ent, dx, dy);

		if (move.time < 1.0) {
			body->x += dx * move.time;
			body->y += dy * move.time;

			if (move.hit.normal.x != 0) {
				dx = 0;
			}

			if (move.hit.normal.y != 0) {
				dy = 0;
			}

			dx *= 1.0 - move.time;
			dy *= 1.0 - move.time;

			auto dotprod = (dx * move.hit.normal.y + dy * move.hit.normal.x) * (1.0 - move.time);
			dx = dotprod * move.hit.normal.y;
			dy = dotprod * move.hit.normal.x;

			move = Move(&es, ent, dx, dy);
		}

		body->x += dx * move.time;
		body->y += dy * move.time;

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

