#include <entityx/entityx.h>
#include "systems_upd.h"
#include "../components.h"

void InputSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<PlayerInput>()) {
		auto input = ent.component<PlayerInput>();
		if (consoleActive) {
			input->left = false;
			input->right = false;
			input->up = false;
			input->down = false;
			input->jump = false;
			input->attack = false;
			input->menu = false;
			continue;
		}
		input->left = CL_KeyState(&in_1_left) > 0;
		input->right = CL_KeyState(&in_1_right) > 0;
		input->up = CL_KeyState(&in_1_up) > 0;
		input->down = CL_KeyState(&in_1_down) > 0;
		input->jump = CL_KeyState(&in_1_jump) > 0;
		input->attack = CL_KeyState(&in_1_attack) > 0;
		input->menu = CL_KeyState(&in_1_menu) > 0;
	}
}