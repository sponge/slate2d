#include "scene_testbounce.h"
#include "pecs.h"
#include "sweep.h"
#include "game.h"
#include "components.h"
#include "componenthelpers.h"
#include "lua_extstate.h"
#include "drawcommands.h"

using namespace pecs;

struct RectMoverSystem : system_t {
	RectMoverSystem() {
		this->name = "Rect Mover (C)";
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

			Sweep move = Trace(*world, entity, dx, dy);

			body.x = move.pos.x;
			body.y = move.pos.y;

			if (move.hit.valid) {
				auto dotprod = (dx * move.hit.normal.y + dy * move.hit.normal.x) * (1.0f - move.time);
				dx = dotprod * move.hit.normal.y;
				dy = dotprod * move.hit.normal.x;

				move = Trace(*world, entity, dx, dy);
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

	RectDrawSystem() {
		this->name = "Rect Drawer";
		this->priority = 0;
		this->renderOnly = true;
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

			DC_SetColor(r.r, r.g, r.b, r.a);
			DC_DrawRect(body.x - (body.w*0.5), body.y - (body.h*0.5), body.w, body.h);

			DC_SetColor(255, 255, 255, 255);
			DC_DrawText(body.x, body.y, va("%.0f, %.0f", body.x, body.y), 2);
			DC_DrawText(body.x, body.y + 12, va("%.0f, %.0f", m.dx, m.dy), 2);
		}
	}
};

// scene

void TestBounceScene::Startup(ClientInfo* info) {
	inf = info;

	world = new BaseWorld();

	auto b = new RectDrawSystem();
	world->add(b);

	//auto a = new RectMoverSystem();
	//world->add(a);

	lua.LoadGameFile("scripts/main_testbounce.lua");
}

void TestBounceScene::Update(float dt) {
	world->update(dt);
}

void TestBounceScene::Render() {
	DC_Clear();
	world->render(0);
	DC_Submit();
}

TestBounceScene::~TestBounceScene() {
	delete world;
}