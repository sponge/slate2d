#include "scene_testbounce.h"
#include <nanovg.h>
#include <random>
#include <tmx.h>
#include <entityx/entityx.h>

#include "sys/systems_drw.h"
//#include "sys/systems_upd.h"
#include "components.h"

namespace ex = entityx;

class TestBounceWorld : public ex::EntityX {
public:
  explicit TestBounceWorld() {
    systems.configure();

		std::random_device rand_dev;
		std::mt19937 g(rand_dev());
		std::uniform_real_distribution<double> distr(0, 1);

		for (int i = 0; i < 16; i++) {
			double x = (i % 4) * (1280 / 4);
			double y = floor(i / 4) * (720 / 4);
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
ex::SystemManager *rendSys;

void TestBounceScene::Startup(ClientInfo* info) {
	inf = info;

	world = new TestBounceWorld();

	rendSys = new ex::SystemManager(world->entities, world->events);
	rendSys->add<RectDrawSystem>(inf);
	rendSys->configure();
}

void TestBounceScene::Update(double dt) {
	world->update(dt);
}

void TestBounceScene::Render() {
	rendSys->update_all(0);
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