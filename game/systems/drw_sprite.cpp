#include "systems.h"
#include "../baseworld.h"
#include "../../src/image.h"
#include "../public.h"

SpriteDrawerSystem::SpriteDrawerSystem(ClientInfo *inf) : inf(inf)
{
	this->name = "Sprite Drawer";
	this->priority = 0;
	this->mask = COMPONENT_BODY | COMPONENT_SPRITE;
	this->renderOnly = true;
}

void SpriteDrawerSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;

		nvgSave(inf->nvg);

		auto &body = world->getBody(entity.id);
		auto &spr = world->getSprite(entity.id);

		nvgTranslate(inf->nvg, body.x, body.y);
		nvgScale(inf->nvg, spr.flipX ? -1 : 1, spr.flipY ? -1 : 1);
		nvgTranslate(inf->nvg, - (spr.size.x / 2), - (spr.size.y / 2));

		if (entity.mask & COMPONENT_ANIMATION) {
			auto &anim = world->getAnimation(entity.id);
			spr.frame = (unsigned int)floor((world->time - anim.startTime) / anim.delay) % (anim.endFrame - anim.startFrame + 1) + anim.startFrame;
		}

        auto img = trap->Img_Get(spr.img);
		auto paint = nvgImagePattern(inf->nvg, 0 - spr.ofs.x - (spr.frame * spr.size.x), 0 - spr.ofs.y, img->w, img->h, 0, img->hnd, 1.0f);
		nvgBeginPath(inf->nvg);
		nvgRect(inf->nvg, 0 - spr.ofs.x, 0 - spr.ofs.y, spr.size.x, spr.size.y);
		nvgFillPaint(inf->nvg, paint);
		nvgFill(inf->nvg);

		nvgRestore(inf->nvg);
	}
}