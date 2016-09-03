#include "systems.h"

void InputSystem::update(EntityManager & es, double dt) {
	auto *state = SDL_GetKeyboardState(NULL);

	for (auto ent : es.entities_with_components<PlayerInput>()) {
		auto input = ent.component<PlayerInput>();
		input->left = state[SDL_SCANCODE_LEFT] != 0;
		input->right = state[SDL_SCANCODE_RIGHT] != 0;
		input->up = state[SDL_SCANCODE_UP] != 0;
		input->down = state[SDL_SCANCODE_DOWN] != 0;
		input->jump = state[SDL_SCANCODE_Z] != 0;
		input->attack = state[SDL_SCANCODE_X] != 0;
		input->run = state[SDL_SCANCODE_A] != 0;
		input->menu = state[SDL_SCANCODE_RETURN] != 0;
	}
}