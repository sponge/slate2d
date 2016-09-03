#include "systems.h"

void PlayerSystem::update(EntityManager & es, double dt) {
	auto *state = SDL_GetKeyboardState(NULL);

	for (auto ent : es.entities_with_components<PlayerInput, Body, Movable>()) {
		auto input = ent.component<PlayerInput>();
		auto body = ent.component<Body>();
		auto speed = ent.component<Movable>();

		speed->dx = input->right ? 50 : input->left ? -50 : 0;
		speed->dy = input->down ? 50 : input->up ? -50 : 0;

		Entity hitEnt;

		auto move = Move(&es, ent, speed->dx * dt, speed->dy * dt, hitEnt);

		body->x = move.pos.x;
		body->y = move.pos.y;

		if (move.hit.valid) {

		}
	}
}