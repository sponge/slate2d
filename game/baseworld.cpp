#include "baseworld.h"
#include "lua_extstate.h"
#include "lua_pecs_system.h"
#include "componenthelpers.h"
#include <tmx.h>

bool BaseWorld::add_lua_system(sol::table opts) {
	sol::function procFunc = opts["process"];
	if (!procFunc.valid()) {
		return false;
	}

	int priority = opts.get_or("priority", 0);
	std::string name = opts.get_or<std::string>("name", "Lua System");
	int mask = COMPONENT_ANY;

	sol::object components = opts["components"];
	if (components.is<sol::table>()) {
		for (auto key : components.as<sol::table>()) {
			mask += key.second.as<int>();
		}
	}
	else if (components.is<int>()) {
		mask = components.as<int>();
	}

	auto sys = new LuaSystem(lua, name.c_str(), priority, mask, procFunc);
	this->add(sys);
	return true;
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
		"master_entity", &BaseWorld::masterEntity,

		"add_system", &BaseWorld::add_lua_system,
		"add_entity", &BaseWorld::add_entity,
		"new_entity", &BaseWorld::get_entity,
		"trace", &BaseWorld::trace,

		"addBody", &BaseWorld::addBody,
		"getBody", &BaseWorld::getBody,
		"addMovable", &BaseWorld::addMovable,
		"getMovable", &BaseWorld::getMovable,
		"addRenderable", &BaseWorld::addRenderable,
		"getRenderable", &BaseWorld::getRenderable,
		"addTileMap", &BaseWorld::addTileMap,
		"getTileMap", &BaseWorld::getTileMap,
		"addCamera", &BaseWorld::addCamera,
		"getCamera", &BaseWorld::getCamera,
		"addPlayerInput", &BaseWorld::addPlayerInput,
		"getPlayerInput", &BaseWorld::getPlayerInput,
		"addPlayer", &BaseWorld::addPlayer,
		"getPlayer", &BaseWorld::getPlayer,
		"addSprite", &BaseWorld::addSprite,
		"getSprite", &BaseWorld::getSprite
		);

	lua.new_usertype<entity_t>("Entity",
		"alive", &entity_t::alive,
		"id", &entity_t::id,
		"mask", &entity_t::mask
	);

	// other types

	lua.new_usertype<tmx_map>("TmxMap",
		"w", &tmx_map::width,
		"h", &tmx_map::height,
		"tile_width", &tmx_map::tile_width,
		"tile_height", &tmx_map::tile_height
		);

	lua.new_usertype<tmx_object>("TmxObject",
		"id", &tmx_object::id,
		"x", &tmx_object::x,
		"y", &tmx_object::y,
		"width", &tmx_object::width,
		"height", &tmx_object::height,
		"gid", &tmx_object::gid,
		"visible", &tmx_object::visible,
		"rotation", &tmx_object::rotation,
		"name", &tmx_object::name,
		"type", &tmx_object::type
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

	lua.new_usertype<TileMap>("TileMap",
		"map", &TileMap::map
		);

	lua.new_usertype<Camera>("Camera",
		sol::constructors<Camera(), Camera(float, float, float, float, float)>(),
		"active", &Camera::active,
		"pos", &Camera::pos,
		"size", &Camera::size,
		"max", &Camera::max,
		"scale", &Camera::scale,
		"top", &Camera::top,
		"right", &Camera::right,
		"bottom", &Camera::bottom,
		"left", &Camera::left,
		"target", &Camera::target,

		"Center", &Camera::Center,
		"Move", &Camera::Move,
		"Bind", &Camera::Bind
		);

	lua.new_usertype<PlayerInput>("PlayerInput",
		sol::constructors<Camera()>(),
		"up", &PlayerInput::up,
		"down", &PlayerInput::down,
		"left", &PlayerInput::left,
		"right", &PlayerInput::right,
		"jump", &PlayerInput::jump,
		"attack", &PlayerInput::attack,
		"menu", &PlayerInput::menu
		);
}