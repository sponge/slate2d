#include "systems.h"
#include "../baseworld.h"
#include "../../src/image.h"
#include "../public.h"
#include "../game.h"

PlayerInputSystem::PlayerInputSystem()
{
	this->name = "Player Input";
	this->priority = 0;
	this->mask = COMPONENT_PLAYERINPUT;
	this->renderOnly = false;
}

void PlayerInputSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;

		auto &input = world->PlayerInputs[entity.id];

		if (input.enabled == false) {
			input.left = false;
			input.right = false;
			input.up = false;
			input.down = false;
			input.jump = false;
			input.attack = false;
			input.menu = false;
			continue;
		}

		input.left = trap->CL_KeyState(&in_1_left) > 0;
		input.right = trap->CL_KeyState(&in_1_right) > 0;
		input.up = trap->CL_KeyState(&in_1_up) > 0;
		input.down = trap->CL_KeyState(&in_1_down) > 0;
		input.jump = trap->CL_KeyState(&in_1_jump) > 0;
		input.attack = trap->CL_KeyState(&in_1_attack) > 0;
		input.menu = trap->CL_KeyState(&in_1_menu) > 0;
	}
}