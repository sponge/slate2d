#include "systems.h"

void CameraDrawSystem::render(EntityManager & es, ClientInfo * inf) {
	for (auto ent : es.entities_with_components<TileMap>()) {
		auto cam = ent.component<Camera>();
		if (cam->active == false) {
			continue;
		}
		nvgScale(inf->nvg, 3, 3);
		nvgTranslate(inf->nvg, 0 - cam->pos.x + 100, 0 - cam->pos.y + 100);
	}
}