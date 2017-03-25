#pragma once
#include "pecs.h"
#include "sol.h"

struct LuaSystem : pecs::system_t {
	sol::state &lua;
	sol::function luaUpdate;

	LuaSystem(sol::state &st, int priority, int mask, sol::function func);
	void update(double dt);
	void on_add(pecs::world_t *world);
	void on_remove(pecs::world_t *world);
	void on_add(pecs::entity_t *entity);
	void on_remove(pecs::entity_t *entity);
};