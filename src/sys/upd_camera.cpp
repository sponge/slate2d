#include "systems.h"

void CameraUpdateSystem::update(EntityManager &es, double dt) {
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

