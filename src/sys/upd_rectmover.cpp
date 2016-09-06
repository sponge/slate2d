#include "systems.h"

const double WORLD_WIDTH = 1280.0;
const double WORLD_HEIGHT = 720.0;

void RectMoverSystem::update(EntityManager &es, double dt) {
	dt = dt > 0.1 ? 0.1 : dt;

	for (auto ent : es.entities_with_components<Body, Movable>()) {
		auto body = ent.component<Body>();
		auto movable = ent.component<Movable>();

		double dx = movable->dx * dt;
		double dy = movable->dy * dt;

		Entity hitEnt;

		auto move = Move(&es, ent, dx, dy, hitEnt);

		body->pos.x = move.pos.x;
		body->pos.y = move.pos.y;

		if (move.hit.valid) {
			auto dotprod = (dx * move.hit.normal.y + dy * move.hit.normal.x) * (1.0 - move.time);
			dx = dotprod * move.hit.normal.y;
			dy = dotprod * move.hit.normal.x;

			move = Move(&es, ent, dx, dy, hitEnt);
			body->pos.x = move.pos.x;
			body->pos.y = move.pos.y;

			// if the second move is still blocked, reverse direction
			if (dx != 0 && move.hit.normal.x != 0) {
				movable->dx *= -1;
			}
			
			if (dy != 0 && move.hit.normal.y != 0) {
				movable->dy *= -1;
			}
		}

		if (body->max().x > WORLD_WIDTH || body->min().x < 0) {
			body->pos.x = body->min().x < 0 ? body->size.x / 2 : WORLD_WIDTH - body->size.x / 2;
			movable->dx *= -1;
		}

		if (body->max().y > WORLD_HEIGHT || body->min().y < 0) {
			body->pos.y = body->min().y < 0 ? body->size.y / 2 : WORLD_HEIGHT - body->size.y / 2;
			movable->dy *= -1;
		}

	}
}

