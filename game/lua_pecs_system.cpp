#include "lua_pecs_system.h"
#include "baseworld.h"
#include "public.h"

using namespace pecs;

LuaSystem::LuaSystem(sol::state &st, std::string name, int priority, int mask, bool renderOnly, sol::function func) : lua(st), luaUpdate(func)
{
	this->name = name;
	this->priority = priority;
	this->mask = mask;
	this->renderOnly = renderOnly;
}

void LuaSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;

		// if onlyEntId is set to an entity number, skip if entity doesn't match
		// FIXME: pull this out so we don't loop through all entities in these systems
		if (this->onlyEntId != -1 && this->onlyEntId != entity.id) {
			continue;
		}

		// override mask to get all components if this lua system only applies to one entity
		int mask = this->mask;
		if (this->onlyEntId != -1) {
			mask = entity.mask;
		}

		// crate a table containing references to all components so lua can update them.
		auto components = lua.create_table_with();
		if (mask & COMPONENT_BODY) { components["body"] = &world->getBody(entity.id); }
		if (mask & COMPONENT_MOVABLE) { components["mov"] = &world->getMovable(entity.id); }
		if (mask & COMPONENT_RENDERABLE) { components["renderable"] = &world->getRenderable(entity.id); }
		if (mask & COMPONENT_TILEMAP) { components["tilemap"] = &world->getTileMap(entity.id); }
		if (mask & COMPONENT_CAMERA) { components["camera"] = &world->getCamera(entity.id); }
		if (mask & COMPONENT_PLAYERINPUT) { components["input"] = &world->getPlayerInput(entity.id); }
		if (mask & COMPONENT_LUATABLE) { components["table"] = world->getTable(entity.id); } // reference the table directly since only member in component
		if (mask & COMPONENT_SPRITE) { components["sprite"] = &world->getSprite(entity.id); }
		if (mask & COMPONENT_ANIMATION) { components["animation"] = &world->getAnimation(entity.id); }
		if (mask & COMPONENT_TRIGGER) { components["trigger"] = &world->getTrigger(entity.id); }

		// run lua update and disable system if lua errors out instead of crashing or spamming every frame.
		auto result = this->luaUpdate(dt, entity, components);
		if (result.valid() == false) {
			sol::error err = result;
			trap->Print("error: (%s) %s", this->name.c_str(), err.what());
			this->active = false;
		}
	}
}

void LuaSystem::on_add(world_t * world)
{
}

void LuaSystem::on_remove(world_t * world)
{
}

void LuaSystem::on_add(entity_t * entity)
{
}

void LuaSystem::on_remove(entity_t * entity)
{
}