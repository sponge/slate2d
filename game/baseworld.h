#pragma once
#include "pecs.h"
#include "components.h"
#include "sol.h"

#define GENERATE_COMPONENT(ENUM, TYPE) \
std::vector<TYPE> TYPE##s; \
void assign(entity_t &entity, TYPE component) { \
	entity.mask |= ENUM ; \
	resize(this->TYPE##s, entity.id); \
	this->TYPE##s[entity.id] = component; \
} \
TYPE & get##TYPE (int id) { \
	return TYPE##s[id]; \
} \
void add##TYPE (entity_t &entity, TYPE comp) { \
	this->assign(entity, comp); \
}

using namespace pecs;

// Resize components as more entities flood in
template <typename T>
void resize(std::vector<T> &components, uint32_t id) {
	size_t c = components.capacity();

	while (c <= id) {
		c = std::max<size_t>(c, 1);
		c = c << 1;
	}

	if (components.size() <= id) {
		components.reserve(c);
		components.resize(id + 1);
	}
}

struct BaseWorld : world_t {
	BaseWorld();

	// update me to generate getters/setters for each component
	GENERATE_COMPONENT(COMPONENT_BODY, Body);
	GENERATE_COMPONENT(COMPONENT_MOVABLE, Movable);
	GENERATE_COMPONENT(COMPONENT_RENDERABLE, Renderable);
	GENERATE_COMPONENT(COMPONENT_TILEMAP, TileMap);
	GENERATE_COMPONENT(COMPONENT_CAMERA, Camera);
	GENERATE_COMPONENT(COMPONENT_PLAYERINPUT, PlayerInput);
	GENERATE_COMPONENT(COMPONENT_SPRITE, Sprite);
	GENERATE_COMPONENT(COMPONENT_ANIMATION, Animation);
	GENERATE_COMPONENT(COMPONENT_TRIGGER, Trigger);
	
	// not using the macro for COMPONENT_LUATABLE so we can pass a table directory in the lua system
	std::vector<sol::table> LuaTables;
	sol::table & getTable(int id) {
		return LuaTables[id];
	}
	void addTable(entity_t &entity, sol::table t) {
		entity.mask |= COMPONENT_LUATABLE;
		resize(this->LuaTables, entity.id);
		this->LuaTables[entity.id] = t;
	}

	// absolute time world has been alive for
	double time = 0;

	bool entityHas(entity_t &ent, int component) {
		return (ent.mask & component) != 0;
	}

	// lua: add a lua-based system
	bool add_lua_system(sol::table opts);
	// lua: adds the entity to the world
	void add_entity(entity_t ent);
	// lua: returns the result of a trace query against the world
	Sweep trace(entity_t &ent, double dx, double dy);
	// lua: returns the first entity with a trigger component that ent is touching
	const entity_t* check_trigger(entity_t &ent);
	// lua: every ecs system needs a master entity that has a bunch of stuff, and this one is no exception
	entity_t * get_master_entity();
	unsigned int masterEntity;

};