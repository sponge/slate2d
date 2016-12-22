#include <entityx/entityx.h>
#include "systems_upd.h"
#include "../components.h"

void PlayerSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<PlayerInput, Body, Movable>()) {
		auto input = ent.component<PlayerInput>();
		auto body = ent.component<Body>();
		auto speed = ent.component<Movable>();

		speed->dx += input->right ? 0.5 : input->left ? -0.5 : (speed->dx > 0 ? -0.5 : speed->dx < 0 ? 0.5 : 0);
		speed->dy += input->down ? 0.5 : input->up ? -0.5 : (speed->dy > 0 ? -0.5 : speed->dy < 0 ? 0.5 : 0);

		if (fabs(speed->dx) < 0.2) {
			speed->dx = 0;
		}

		if (fabs(speed->dy) < 0.2) {
			speed->dy = 0;
		}

		ex::Entity hitEnt;

		auto move = Move(&es, ent, speed->dx * dt, speed->dy * dt, hitEnt);

		body->pos.x = move.pos.x;
		body->pos.y = move.pos.y;

		if (move.hit.valid) {

		}
	}
}