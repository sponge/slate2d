#include "systems.h"
#include "../baseworld.h"
#include "../../src/image.h"
#include "../public.h"

RectDrawerSystem::RectDrawerSystem(ClientInfo *inf) : inf(inf)
{
	this->name = "Rect Drawer";
	this->priority = 0;
	this->mask = COMPONENT_BODY | COMPONENT_MOVABLE | COMPONENT_RENDERABLE;
	this->renderOnly = true;
}

void RectDrawerSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;

		auto &body = world->getBody(entity.id);
		auto &m = world->getMovable(entity.id);
		auto &r = world->getRenderable(entity.id);

		nvgBeginPath(inf->nvg);
		nvgRect(inf->nvg, body.x - (body.w*0.5), body.y - (body.h*0.5), body.w, body.h);
		nvgFillColor(inf->nvg, nvgRGBA(r.r, r.g, r.b, r.a));
		nvgFill(inf->nvg);

		nvgFillColor(inf->nvg, nvgRGBA(255, 255, 255, 255));
		char s[16];
		nvgTextAlign(inf->nvg, 2);
		snprintf(s, sizeof(s), "%.0f, %.0f", body.x, body.y);
		nvgText(inf->nvg, body.x, body.y, s, 0);
		snprintf(s, sizeof(s), "%.0f, %.0f", m.dx, m.dy);
		nvgText(inf->nvg, body.x, body.y + 12, s, 0);
	}

}