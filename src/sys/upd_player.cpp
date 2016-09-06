#include "systems.h"

void PlayerSystem::update(EntityManager & es, double dt) {
	auto *state = SDL_GetKeyboardState(NULL);

	for (auto ent : es.entities_with_components<PlayerInput, Body, Movable>()) {
		auto input = ent.component<PlayerInput>();
		auto body = ent.component<Body>();
		auto speed = ent.component<Movable>();

		speed->dx += input->right ? 0.5 : input->left ? -0.5 : (speed->dx > 0 ? -0.5 : 0.5);
		speed->dy += input->down ? 0.5 : input->up ? -0.5 : (speed->dy > 0 ? -0.5 : 0.5);

		Entity hitEnt;

		auto move = Move(&es, ent, speed->dx * dt, speed->dy * dt, hitEnt);

		body->pos.x = move.pos.x;
		body->pos.y = move.pos.y;

		if (move.hit.valid) {

		}
	}
}