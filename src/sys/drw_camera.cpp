#include <entityx/entityx.h>
#include "systems_drw.h"
#include "../components.h"

void CameraDrawSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<TileMap>()) {
		auto cam = ent.component<Camera>();
		if (cam->active == false) {
			continue;
		}
		
		nvgTransform(inf->nvg, cam->scale, 0, 0, cam->scale, (0 - cam->pos.x * cam->scale), 0 - cam->pos.y * cam->scale);
	}
}