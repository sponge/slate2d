#include "systems.h"
#include <stdio.h>

void RectDrawSystem::render(EntityManager &es, ClientInfo *inf) {
	NVGcontext *nvg = inf->nvg;

	for (auto ent : es.entities_with_components<Body, Renderable>()) {
		auto body = ent.component<Body>();
		auto r = ent.component<Renderable>();

		nvgBeginPath(nvg);
		nvgRect(nvg, body->pos.x - (body->size.x*0.5), body->pos.y - (body->size.y*0.5), body->size.x, body->size.y);
		nvgFillColor(nvg, nvgRGBA(r->r, r->g, r->b, r->a));
		nvgFill(nvg);

		nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255));
		char s[16];
		auto m = ent.component<Movable>();
		nvgTextAlign(nvg, 2);
		snprintf(s, sizeof(s), "%.0f, %.0f", body->pos.x, body->pos.y);
		nvgText(nvg, body->pos.x, body->pos.y, s, 0);
		snprintf(s, sizeof(s), "%.0f, %.0f", m->dx, m->dy);
		nvgText(nvg, body->pos.x, body->pos.y + 12, s, 0);
	}
}

