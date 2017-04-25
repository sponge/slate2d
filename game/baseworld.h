#pragma once
#include "pecs.h"
#include "components.h"
#include "sol.h"

#define GENERATE_COMPONENT(ENUM, TYPE) \
std::vector<TYPE> TYPE##s; \
void assign(entity_t *entity, TYPE component) { \
	entity->mask |= ENUM ; \
	resize(this->TYPE##s, entity->id); \
	this->TYPE##s[entity->id] = component; \
} \
TYPE & get##TYPE (int id) { \
	return TYPE##s[id]; \
} \
void add##TYPE (entity_t *entity, TYPE comp) { \
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
	GENERATE_COMPONENT(COMPONENT_BODY, Body);
	GENERATE_COMPONENT(COMPONENT_MOVABLE, Movable);
	GENERATE_COMPONENT(COMPONENT_RENDERABLE, Renderable);
	GENERATE_COMPONENT(COMPONENT_TILEMAP, TileMap);
	GENERATE_COMPONENT(COMPONENT_CAMERA, Camera);
	GENERATE_COMPONENT(COMPONENT_PLAYERINPUT, PlayerInput);
	GENERATE_COMPONENT(COMPONENT_SPRITE, Sprite);
	GENERATE_COMPONENT(COMPONENT_ANIMATION, Animation);
	GENERATE_COMPONENT(COMPONENT_TRIGGER, Trigger);

	// not using the macro so we can pass a table directory
	std::vector<sol::table> LuaTables;
	sol::table & getTable(int id) {
		return LuaTables[id];
	}
	void addTable(entity_t *entity, sol::table t) {
		entity->mask |= COMPONENT_LUATABLE;
		resize(this->LuaTables, entity->id);
		this->LuaTables[entity->id] = t;
	}

	entity_t * get_master_entity();
	unsigned int masterEntity;
	double time = 0;

	// functions exposed to lua environment
	bool add_lua_system(sol::table opts);
	void add_entity(entity_t ent);
	int new_image(const char *name, const char *path);
	Sweep trace(entity_t &ent, double dx, double dy);
	const entity_t* check_trigger(entity_t &ent);
	void debug_text(const char *text);
};