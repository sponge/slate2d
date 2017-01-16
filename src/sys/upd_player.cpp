#include <entityx/entityx.h>
#include "systems_upd.h"
#include "../components.h"
#include "../cvar_game.h"

void PlayerSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<Player, PlayerInput, Body, Movable>()) {
		auto player = ent.component<Player>();
		auto input = ent.component<PlayerInput>();
		auto body = ent.component<Body>();
		auto mov = ent.component<Movable>();

		ex::Entity touchEnt;
		mov->rightTouch = Trace(es, ent, 1, 0, touchEnt).time < 1e-7;
		mov->leftTouch = Trace(es, ent, -1, 0, touchEnt).time < 1e-7;
		mov->downTouch = Trace(es, ent, 0, 1, touchEnt).time < 1e-7;
		mov->upTouch = Trace(es, ent, 0, -1, touchEnt).time < 1e-7;

		//ImGui::Text("%i, %i, %i, %i\n", mov->rightTouch, mov->leftTouch, mov->downTouch, mov->upTouch);

		// if they're on the ground, they can always jump
		if (mov->downTouch) {
			player->numJumps = 0;
		}

        player->isWallSliding = false;

		// if they're midair and are holding down toward the wall, they can wall jump
		player->canWallJump = !mov->downTouch && ((input->left && mov->leftTouch) || (input->right && mov->rightTouch));

		// if they're moving down and touching a wall the direction they're holding down, they are wall sliding
        if (mov->dy > 0 && player->canWallJump) {
            player->isWallSliding = true;
        }

		// finished updating player, now start figuring out speeds

		mov->dy += p_gravity->value * dt;

		if (player->isWallSliding) {
			mov->dy = p_wallSlideSpeed->value;
		}

		if (input->right || input->left) {
			mov->dx += (input->right ? p_accel->value : input->left ? -p_accel->value : 0) * dt;
		} else if (mov->dx != 0) {
			auto friction = p_groundFriction->value * dt;
			if (friction > fabs(mov->dx)) {
				mov->dx = 0;
			} else {
				mov->dx += friction * (mov->dx > 0 ? -1 : 1);
			}
		}

		if (input->up || input->down) {
			mov->dy = input->down ? p_accel->value : input->up ? -p_accel->value : 0 * dt;
		}

		mov->dx = clamp(mov->dx, -p_maxSpeed->value, p_maxSpeed->value);
		auto uncappedY = mov->dy;
		mov->dy = clamp(mov->dy, -p_terminalVelocity->value, p_terminalVelocity->value);

		// do the move and collision checks
		ex::Entity hitEnt;

		// do x and y separately since otherwise you get flickering collision normals
		// it seems to work better for continuous input actions
		Sweep xmove = Trace(es, ent, mov->dx * dt, 0, hitEnt);
		body->pos.x = xmove.pos.x;
		if (xmove.hit.valid) {
			mov->dx = 0;
		}

		Sweep ymove = Trace(es, ent, 0, mov->dy * dt, hitEnt);
		body->pos.y = ymove.pos.y;
		if (ymove.hit.valid) {
			mov->dy = 0;
		}

		if (fabs(mov->dx) < 0.2) {
			mov->dx = 0;
		}

		if (fabs(mov->dy) < 0.2) {
			mov->dy = 0;
		}
	}
}