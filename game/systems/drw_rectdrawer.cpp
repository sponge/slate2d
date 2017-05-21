#include "systems.h"
#include "../baseworld.h"
#include "../cvar_game.h"
#include "../drawcommands.h"
#include "../game.h"

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

	if (dbg_drawBbox->integer == 0) {
		return;
	}

	for (auto &entity : world->entities) {
		for (auto &entity : world->entities) {
			PECS_SKIP_INVALID_ENTITY;

			auto &body = world->getBody(entity.id);
			auto &m = world->getMovable(entity.id);
			auto &r = world->getRenderable(entity.id);

			DC_SetColor(r.r, r.g, r.b, r.a);
			DC_DrawRect(body.x - (body.w*0.5), body.y - (body.h*0.5), body.w, body.h);

			DC_SetColor(255, 255, 255, 255);
			DC_DrawText(body.x, body.y, va("%.0f, %.0f", body.x, body.y), 2);
			DC_DrawText(body.x, body.y + 12, va("%.0f, %.0f", m.dx, m.dy), 2);
		}
	}
}