#pragma once
#include "pecs.h"
#include "selene/selene.h"

struct LuaSystem : pecs::system_t {
	sel::State &lua;
	sel::function<int(lua_Number, int)> luaUpdate;

	LuaSystem(sel::State &st, int priority, int mask, sel::function<int(lua_Number, int)> func);
	void update(double dt);
	void on_add(pecs::world_t *world);
	void on_remove(pecs::world_t *world);
	void on_add(pecs::entity_t *entity);
	void on_remove(pecs::entity_t *entity);
};