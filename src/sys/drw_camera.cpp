#include "systems.h"

void CameraSystem::render(EntityManager & es, ClientInfo * inf) {
	for (auto ent : es.entities_with_components<TileMap>()) {
		auto cam = ent.component<Camera>();
		if (cam->active == false) {
			continue;
		}
		nvgTranslate(inf->nvg, 0 - cam->pos.x, 0 - cam->pos.y);
	}
}