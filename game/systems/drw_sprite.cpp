#include "systems.h"
#include "../baseworld.h"
#include "../public.h"
#include "../componenthelpers.h"

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

		auto &body = world->getBody(entity.id);
		auto &spr = world->getSprite(entity.id);

		if (entity.mask & COMPONENT_ANIMATION) {
			auto &anim = world->getAnimation(entity.id);
			spr.frame = (unsigned int)floor((world->time - anim.startTime) / anim.delay) % (anim.endFrame - anim.startFrame + 1) + anim.startFrame;
		}

		DC_DrawSprite(body, spr);
	}
}