#include <entityx/entityx.h>
#include "systems_drw.h"
#include "../components.h"

void CameraDrawSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	for (auto ent : es.entities_with_components<TileMap>()) {
		auto cam = ent.component<Camera>();
		if (cam->active == false) {
			continue;
		}
		nvgScale(inf->nvg, 3, 3);
		nvgTranslate(inf->nvg, 0 - cam->pos.x + 100, 0 - cam->pos.y + 100);
	}
}