#pragma once
#include "pecs.h"
#include "sol.h"

struct LuaSystem : pecs::system_t {
	sol::state &lua;
	sol::protected_function luaUpdate;
	int onlyEntId = -1;

	LuaSystem(sol::state &st, std::string name, int priority, int mask, bool renderOnly, sol::function func);
	void update(double dt);
	void on_add(pecs::world_t *world);
	void on_remove(pecs::world_t *world);
	void on_add(pecs::entity_t *entity);
	void on_remove(pecs::entity_t *entity);
};