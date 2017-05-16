#include "systems.h"
#include "../baseworld.h"
#include "../shared.h"

CameraDrawSystem::CameraDrawSystem(ClientInfo *inf) : inf(inf)
{
	this->name = "Draw Camera";
	this->priority = 0;
	this->mask = COMPONENT_CAMERA;
	this->renderOnly = true;
}

void CameraDrawSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;

		auto &cam = world->Cameras[entity.id];

		if (cam.active == false) {
			continue;
		}

		nvgTransform(inf->nvg, cam.scale, 0, 0, cam.scale, (0 - cam.pos.x * cam.scale), 0 - cam.pos.y * cam.scale);
	}
}