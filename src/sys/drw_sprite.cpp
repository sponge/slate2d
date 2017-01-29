#include <entityx/entityx.h>
#include "systems_drw.h"
#include "../components.h"

void SpriteDrawSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	NVGcontext *nvg = inf->nvg;

	nvgSave(nvg);

	es.each<Body, Sprite>([this, nvg](ex::Entity entity, Body &body, Sprite &sprite) {
		nvgTranslate(nvg, body.pos.x - (sprite.size.x / 2), body.pos.y - (sprite.size.y / 2));

		auto paint = nvgImagePattern(nvg, 0 - sprite.ofs.x, 0 - sprite.ofs.y, sprite.img->w, sprite.img->h, 0, sprite.img->hnd, 1.0f);
		nvgBeginPath(nvg);
		nvgRect(nvg, 0 - sprite.ofs.x, 0 - sprite.ofs.y, sprite.size.x, sprite.size.y);
		nvgFillPaint(nvg, paint);
		nvgFill(nvg);
		nvgRestore(nvg);
	});
}