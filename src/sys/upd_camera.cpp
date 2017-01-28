#include <entityx/entityx.h>
#include "systems_upd.h"
#include "../components.h"

namespace ex = entityx;

void CameraUpdateSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	es.each<Camera>([this](ex::Entity entity, Camera &cam) {
		if (cam.active == false) {
			return;
		}

		if (cam.target != nullptr) {
			Cam_Center(cam, cam.target->pos.x, cam.target->pos.y);
			Cam_Bind(cam);
		}
	});
}

