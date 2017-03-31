#include "baseworld.h"
#include "lua_extstate.h"
#include "lua_pecs_system.h"
#include "componenthelpers.h"


void BaseWorld::add_lua_system(const char *name, int priority, int mask, sol::function func) {
	auto sys = new LuaSystem(lua, name, priority, mask, func);
	this->add(sys);
}

void BaseWorld::add_entity(entity_t ent) {
	this->add(ent);
}

Sweep BaseWorld::trace(entity_t & ent, double dx, double dy) {
	return Trace(*this, ent, dx, dy, NULL);
}

BaseWorld::BaseWorld() {
	// expose world management to lua

	lua["world"] = this;
	lua.new_usertype<BaseWorld>("BaseWorld",
		"add_system", &BaseWorld::add_lua_system,
		"add_entity", &BaseWorld::add_entity,
		"new_entity", &BaseWorld::get_entity,
		"trace", &BaseWorld::trace,
		"addBody", &BaseWorld::addBody,
		"addMovable", &BaseWorld::addMovable,
		"addRenderable", &BaseWorld::addRenderable
		);

	// collision types

	lua.new_usertype<Vec2>("Vec2",
		"x", &Vec2::x,
		"y", &Vec2::y
		);

	lua.new_usertype<Hit>("Hit",
		"valid", &Hit::valid,
		"pos", &Hit::pos,
		"delta", &Hit::delta,
		"normal", &Hit::normal,
		"time", &Hit::time
		);

	lua.new_usertype<Sweep>("Sweep",
		"hit", &Sweep::hit,
		"pos", &Sweep::pos,
		"time", &Sweep::time
		);

	// components

	lua.new_usertype<Body>("Body",
		sol::constructors<Body(), Body(double, double, double, double)>(),
		"x", &Body::x,
		"y", &Body::y,
		"w", &Body::w,
		"h", &Body::h,
		"hw", &Body::hw,
		"hh", &Body::hh
		);

	lua.new_usertype<Movable>("Movable",
		sol::constructors<Movable(), Movable(double, double)>(),
		"dx", &Movable::dx,
		"dy", &Movable::dy
		);

	lua.new_usertype<Renderable>("Renderable",
		sol::constructors<Renderable(), Renderable(unsigned char, unsigned char, unsigned char, unsigned char)>(),
		"r", &Renderable::r,
		"g", &Renderable::g,
		"b", &Renderable::b,
		"a", &Renderable::a
		);
}