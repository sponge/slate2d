#include <entityx/entityx.h>
#include "systems_upd.h"
#include "../components.h"

void CameraUpdateSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<Camera>()) {
		auto cam = ent.component<Camera>();
		if (cam->active == false) {
			continue;
		}

		if (cam->target != nullptr) {
			cam->Move(cam->target->pos.x, cam->target->pos.y);
		}
	}
}

