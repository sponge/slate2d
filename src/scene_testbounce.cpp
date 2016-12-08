#include "scene_testbounce.h"
#include <nanovg.h>
#include <random>
#include <tmx.h>
#include <entityx/entityx.h>

//#include "sys/systems.h"
#include "components.h"

namespace ex = entityx;

class RectDrawSystem : public ex::System<RectDrawSystem> {
public:
  explicit RectDrawSystem(ClientInfo *inf) : inf(inf) {}

  void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override {
		NVGcontext *nvg = inf->nvg;

		es.each<Body, Renderable, Movable>([this, nvg](ex::Entity entity, Body &body, Renderable &r, Movable &m) {
			nvgBeginPath(nvg);
			nvgRect(nvg, body.pos.x - (body.size.x*0.5), body.pos.y - (body.size.y*0.5), body.size.x, body.size.y);
			nvgFillColor(nvg, nvgRGBA(r.r, r.g, r.b, r.a));
			nvgFill(nvg);

			nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255));
			char s[16];
			nvgTextAlign(nvg, 2);
			snprintf(s, sizeof(s), "%.0f, %.0f", body.pos.x, body.pos.y);
			nvgText(nvg, body.pos.x, body.pos.y, s, 0);
			snprintf(s, sizeof(s), "%.0f, %.0f", m.dx, m.dy);
			nvgText(nvg, body.pos.x, body.pos.y + 12, s, 0);
		});
  }
private:
	ClientInfo *inf;
};

class TestBounceWorld : public ex::EntityX {
public:
  explicit TestBounceWorld(ClientInfo *inf) {
		if (inf != nullptr) {
			systems.add<RectDrawSystem>(inf);
		}
    systems.configure();

		std::random_device rand_dev;
		std::mt19937 g(rand_dev());
		std::uniform_real_distribution<double> distr(0, 1);

		for (int i = 0; i < 16; i++) {
			double x = (i % 4) * (inf->width / 4);
			double y = floor(i / 4) * (inf->height / 4);
			double w = distr(g) * 50 + 40;
			double h = distr(g) * 50 + 40;
			double dx = distr(g) * 200 * (x < 2 ? 1 : -1) + 50;
			double dy = distr(g) * 200 * (y < 2 ? 1 : -1) + 50;

			auto ent = this->entities.create();
			ent.assign<Body>(x, y, w, h);
			ent.assign<Movable>(dx, dy);
			ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);
		}
  }

  void update(ex::TimeDelta dt) {
		systems.update_all(dt);
  }
};

TestBounceWorld *world;

void TestBounceScene::Startup(ClientInfo* info) {
	inf = info;

	world = new TestBounceWorld(inf);
}

void TestBounceScene::Update(double dt) {
	world->update(dt);
}

void TestBounceScene::Render() {
}

void TestBounceScene::Teardown() {
	/*
	for (auto sys : updateSystems) {
		delete(sys);
	}
	updateSystems.clear();

	for (auto sys : renderSystems) {
		delete(sys);

	}
	renderSystems.clear();*/
}