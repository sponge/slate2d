#include "systems.h"
#include <stdio.h>

void RectDrawSystem::render(EntityManager &es, ClientInfo *inf) {
	NVGcontext *nvg = inf->nvg;

	for (auto ent : es.entities_with_components<Body, Renderable>()) {
		auto body = ent.component<Body>();
		auto r = ent.component<Renderable>();

		nvgBeginPath(nvg);
		nvgRect(nvg, body->x - (body->w*0.5), body->y - (body->h*0.5), body->w, body->h);
		nvgFillColor(nvg, nvgRGBA(r->r, r->g, r->b, r->a));
		nvgFill(nvg);

		nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255));
		char s[16];
		auto m = ent.component<Movable>();
		nvgTextAlign(nvg, 2);
		snprintf(s, sizeof(s), "%.0f, %.0f", body->x, body->y);
		nvgText(nvg, body->x, body->y, s, 0);
		snprintf(s, sizeof(s), "%.0f, %.0f", m->dx, m->dy);
		nvgText(nvg, body->x, body->y + 12, s, 0);
	}
}

