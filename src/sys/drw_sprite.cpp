#include <entityx/entityx.h>
#include "systems_drw.h"
#include "../components.h"

void SpriteDrawSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	NVGcontext *nvg = inf->nvg;

	es.each<Body, Sprite>([this, nvg](ex::Entity entity, Body &body, Sprite &sprite) {
		nvgBeginPath(nvg);
		nvgRect(nvg, body.pos.x - (body.size.x*0.5), body.pos.y - (body.size.y*0.5), body.size.x, body.size.y);
		auto paint = nvgImagePattern(nvg, 0, 0, 0, 0, 0, ((Img*)sprite.img)->hnd, 1.0f);
		nvgFill(nvg);
	});
}