#include "scene_testbounce.h"
#include <random>
#include "pecs.h"
#include "../src/sweep.h"
#include "game.h"
#include "components.h"
#include "componenthelpers.h"
#include "lua_extstate.h"
#include "lua_pecs_system.h"

using namespace pecs;

struct RectMoverSystem : system_t {
	RectMoverSystem() {
		this->priority = 0;
		this->mask = COMPONENT_BODY | COMPONENT_MOVABLE;
	}

	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }

	void update(double dt) {
		const float WORLD_WIDTH = 1280.0;
		const float WORLD_HEIGHT = 720.0;
		BaseWorld *world = (BaseWorld*)this->world;

		for (auto &entity : world->entities) {
			PECS_SKIP_INVALID_ENTITY;

			auto &body = world->Bodys[entity.id];
			auto &movable = world->Movables[entity.id];

			float dx = movable.dx * dt;
			float dy = movable.dy * dt;
			
			entity_t *hitEnt = nullptr;

			Sweep move = Trace(*world, entity, dx, dy, hitEnt);

			body.x = move.pos.x;
			body.y = move.pos.y;

			if (move.hit.valid) {
				auto dotprod = (dx * move.hit.normal.y + dy * move.hit.normal.x) * (1.0f - move.time);
				dx = dotprod * move.hit.normal.y;
				dy = dotprod * move.hit.normal.x;

				move = Trace(*world, entity, dx, dy, hitEnt);
				body.x = move.pos.x;
				body.y = move.pos.y;

				// if the second move is still blocked, reverse direction
				if (dx != 0 && move.hit.normal.x != 0) {
					movable.dx *= -1;
				}

				if (dy != 0 && move.hit.normal.y != 0) {
					movable.dy *= -1;
				}
			}

			if (body.max().x > WORLD_WIDTH || body.min().x < 0) {
				body.x = body.min().x < 0 ? body.w / 2 : WORLD_WIDTH - body.w / 2;
				movable.dx *= -1;
			}

			if (body.max().y > WORLD_HEIGHT || body.min().y < 0) {
				body.y = body.min().y < 0 ? body.h / 2 : WORLD_HEIGHT - body.h / 2;
				movable.dy *= -1;
			}
		}
	}
};

struct RectDrawSystem : system_t {
	NVGcontext *nvg;

	RectDrawSystem(NVGcontext *nvg) {
		this->nvg = nvg;
		this->priority = 0;
		this->mask = COMPONENT_BODY | COMPONENT_MOVABLE | COMPONENT_RENDERABLE;
	}

	void on_add(world_t *world) { }
	void on_remove(world_t *world) { }
	void on_add(entity_t *entity) { }
	void on_remove(entity_t *entity) { }

	void update(double dt) {
		BaseWorld *world = (BaseWorld*)this->world;

		for (auto &entity : world->entities) {
			PECS_SKIP_INVALID_ENTITY;

			auto &body = world->getBody(entity.id);
			auto &m = world->getMovable(entity.id);
			auto &r = world->getRenderable(entity.id);

			nvgBeginPath(nvg);
			nvgRect(nvg, body.x - (body.w*0.5), body.y - (body.h*0.5), body.w, body.h);
			nvgFillColor(nvg, nvgRGBA(r.r, r.g, r.b, r.a));
			nvgFill(nvg);

			nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255));
			char s[16];
			nvgTextAlign(nvg, 2);
			snprintf(s, sizeof(s), "%.0f, %.0f", body.x, body.y);
			nvgText(nvg, body.x, body.y, s, 0);
			snprintf(s, sizeof(s), "%.0f, %.0f", m.dx, m.dy);
			nvgText(nvg, body.x, body.y + 12, s, 0);
		}
	}
};

// scene

void TestBounceScene::Startup(ClientInfo* info) {
	inf = info;

	world = new BaseWorld();

	//auto a = new RectMoverSystem();
	//world->add(a);

	auto b = new RectDrawSystem(inf->nvg);
	world->add(b);

	std::random_device rand_dev;
	std::mt19937 g(rand_dev());
	std::uniform_real_distribution<double> distr(0, 1);

	for (int i = 0; i < 16; i++) {
		float x = (i % 4) * (1280 / 4) + 100;
		float y = floor(i / 4) * (720 / 4) + 100;
		float w = distr(g) * 50 + 40;
		float h = distr(g) * 50 + 40;
		float dx = distr(g) * 200 * (x < 2 ? 1 : -1) + 50;
		float dy = distr(g) * 200 * (y < 2 ? 1 : -1) + 50;

		auto ent = world->get_entity();

		world->assign(&ent, Body{ x, y, w, h });
		world->assign(&ent, Movable{ dx, dy });
		world->assign(&ent, Renderable{ (unsigned char) (distr(g) * 255), (unsigned char)(distr(g) * 255), (unsigned char)(distr(g) * 255), (unsigned char)(distr(g) * 200 + 55) });
		world->add(ent);
	}

	
	lua["add_system"] = [this](int priority, int mask, sol::function func) {
		auto sys = new LuaSystem(lua, priority, mask, func);
		world->add(sys);
	};
	
	lua["trace"] = [this](entity_t &ent, double dx, double dy) {
		return Trace(*world, ent, dx, dy, NULL);
	};

	lua.new_usertype<Body>("Body",
		"x", &Body::x,
		"y", &Body::y,
		"w", &Body::w,
		"h", &Body::h,
		"hw", &Body::hw,
		"hh", &Body::hh
	);

	lua.new_usertype<Movable>("Movable",
		"dx", &Movable::dx,
		"dy", &Movable::dy
	);

	lua.new_usertype<Renderable>("Renderable",
		"r", &Renderable::r,
		"g", &Renderable::g,
		"b", &Renderable::b,
		"a", &Renderable::a
	);

	lua.LoadGameFile("scripts/main.lua");
}

void TestBounceScene::Update(float dt) {
	world->update(dt);
}

void TestBounceScene::Render() {
	world->render(0);
}

TestBounceScene::~TestBounceScene() {
	delete world;
}