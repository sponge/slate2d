#include "lua_pecs_system.h"
#include "baseworld.h"
#include "public.h"

using namespace pecs;

LuaSystem::LuaSystem(sol::state &st, int priority, int mask, sol::function func) : luaUpdate(func), lua(st)
{
	this->priority = priority;
	this->mask = mask;
}

void LuaSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;
		auto components = lua.create_named_table("components");
		if (entity.mask & COMPONENT_BODY) {
			auto body = world->getBody(entity.id);
			components["body"] = lua.create_table_with(
				"x", body.x,
				"y", body.y,
				"w", body.w,
				"h", body.h
			);
		}
		if (entity.mask & COMPONENT_MOVABLE) {
			auto mov = world->getMovable(entity.id);
			components["movable"] = lua.create_table_with(
				"dx", mov.dx,
				"dy", mov.dy
			);
		}

		this->luaUpdate(dt, entity.id);

		if (entity.mask & COMPONENT_BODY) {
			auto body = world->getBody(entity.id);
			sol::table tbl = components["body"];
			double x = tbl["x"];
			body.x = tbl["x"];
			body.y = tbl["y"];
			body.w = tbl["w"];
			body.h = tbl["h"];
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