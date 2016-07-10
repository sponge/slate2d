#include "systems.h"

void RectDrawSystem::render(EntityManager &es, ClientInfo *inf) {
	NVGcontext *nvg = inf->nvg;

	for (auto ent : es.entities_with_components<Body, Renderable>()) {
		auto body = ent.component<Body>();
		auto r = ent.component<Renderable>();

		nvgBeginPath(nvg);
		nvgRect(nvg, body->x, body->y, body->w, body->h);
		nvgFillColor(nvg, nvgRGBA(r->r, r->g, r->b, r->a));
		nvgFill(nvg);
	}
}

