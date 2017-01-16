#include <entityx/entityx.h>
#include "systems_upd.h"
#include "../components.h"
#include "../cvar_game.h"

void PlayerSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<PlayerInput, Body, Movable>()) {
		auto input = ent.component<PlayerInput>();
		auto body = ent.component<Body>();
		auto mov = ent.component<Movable>();

		ex::Entity touchEnt;
		mov->rightTouch = Trace(es, ent, 1, 0, touchEnt).time < 1e-7;
		mov->leftTouch = Trace(es, ent, -1, 0, touchEnt).time < 1e-7;
		mov->downTouch = Trace(es, ent, 0, 1, touchEnt).time < 1e-7;
		mov->upTouch = Trace(es, ent, 0, -1, touchEnt).time < 1e-7;

		ImGui::Text("%0.15f", Trace(es, ent, 0, -1, touchEnt).time);
		ImGui::Text("%i, %i, %i, %i\n", mov->rightTouch, mov->leftTouch, mov->downTouch, mov->upTouch);

		mov->dy += p_gravity->value * dt;

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

		Sweep move = Trace(es, ent, mov->dx * dt, mov->dy * dt, hitEnt);

		body->pos.x = move.pos.x;
		body->pos.y = move.pos.y;

		if (move.hit.valid) {
			ImGui::Text("valid hit normal (%0.15f,%0.15f)", move.hit.normal.x, move.hit.normal.y);
			mov->dx = move.hit.normal.x ? 0 : mov->dx;
			mov->dy = move.hit.normal.y ? 0 : mov->dy;
			
			auto remain = 1.0 - move.time;
			auto remx = move.hit.normal.x != 0 ? 0 : mov->dx * dt * remain;
			auto remy = move.hit.normal.y != 0 ? 0 : mov->dy * dt * remain;
			Sweep moveremain = Trace(es, ent, remx, remy, hitEnt);

			body->pos.x = moveremain.pos.x;
			body->pos.y = moveremain.pos.y;
		}

		if (fabs(mov->dx) < 0.2) {
			mov->dx = 0;
		}

		if (fabs(mov->dy) < 0.2) {
			mov->dy = 0;
		}
	}
}