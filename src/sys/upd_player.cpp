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

		if (mov->downTouch && (mov->dy < 0 || !player->willPogo)) {
			player->willPogo = input->down;
		}

		// finished updating player, now start figuring out speeds

		// apply gravity now in case we want to override it later
		mov->dy += p_gravity->value * dt;

		// apply wallslide speed after gravity
		if (player->isWallSliding) {
			mov->dy = p_wallSlideSpeed->value;
		}

		// reset jump and slow upward velocity
		if (!input->jump && player->jumpHeld) {
			player->jumpHeld = false;
			if (mov->dy < 0) {
				mov->dy *= p_earlyJumpEndModifier->value;
			}
		}

		// if touching ground and are about to pogo, pogo is first prio
		if (mov->downTouch && player->willPogo) {
			mov->dy = -p_pogoJumpHeight->value;
			player->numJumps = 1;
			player->willPogo = false;
		}
		// check for various types of jumps
		else if (input->jump && !player->jumpHeld) {
			if (player->canWallJump) {
				mov->dy = -p_doubleJumpHeight->value; // TODO: separate wall jump height?
				mov->dx = p_wallJumpX->value * (input->right ? -1 : 1);
				// FIXME: need global time for stun
				// player->stunTime = time + 0.1f;
				player->jumpHeld = true;
				player->numJumps = 1;
			}
			// regular on ground jump
			else if (mov->downTouch) {
				mov->dy = -(p_jumpHeight->value + (fabs(mov->dx) >= p_maxSpeed->value * 0.25f ? p_speedJumpBonus->value : 0));
				player->jumpHeld = true;
				player->numJumps = 1;
			}
			// midair double jump
			else if (player->numJumps < 2) {
				mov->dy = -p_doubleJumpHeight->value;
				player->numJumps = 2;
				player->jumpHeld = true;
			}
		}

		// FIXME: weapon logic

		if (input->right || input->left) {
			float accel = 0;
			auto isSkidding = (input->left && mov->dx > 0) || (input->right && mov->dx < 0);
			// FIXME: check stun time here
			if (mov->downTouch) {
				accel = isSkidding ? p_skidAccel->value : p_accel->value;
			}
			else {
				accel = isSkidding ? p_turnAirAccel->value : p_airAccel->value;
			}

			mov->dx += (input->right ? accel : input->left ? -accel : 0) * dt;
		} else if (mov->dx != 0) {
			auto friction = p_groundFriction->value * dt;
			if (friction > fabs(mov->dx)) {
				mov->dx = 0;
			} else {
				mov->dx += friction * (mov->dx > 0 ? -1 : 1);
			}
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

		// keep going upward as long as we don't have an upward collision
		if (mov->dy < 0 && !ymove.hit.valid) {
			mov->dy = uncappedY;
		}
	}
}