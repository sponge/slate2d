#include "scene_testbounce.h"
#include <random>
#include <entityx/entityx.h>

#include "sys/systems_drw.h"
#include "sys/systems_upd.h"
#include "components.h"

TestBounceWorld::TestBounceWorld() {
	systems.add<RectMoverSystem>();
	systems.configure();

	std::random_device rand_dev;
	std::mt19937 g(rand_dev());
	std::uniform_real_distribution<double> distr(0, 1);

	for (int i = 0; i < 16; i++) {
		float x = (i % 4) * (1280 / 4);
		float y = floor(i / 4) * (720 / 4);
		float w = distr(g) * 50 + 40;
		float h = distr(g) * 50 + 40;
		float dx = distr(g) * 200 * (x < 2 ? 1 : -1) + 50;
		float dy = distr(g) * 200 * (y < 2 ? 1 : -1) + 50;

		auto ent = this->entities.create();
		ent.assign<Body>(x, y, w, h);
		ent.assign<Movable>(dx, dy);
		ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);
	}
}

void TestBounceWorld::update(ex::TimeDelta dt) {
	systems.update_all(dt);
}

void TestBounceScene::Startup(ClientInfo* info) {
	inf = info;

	world = new TestBounceWorld();

	rendSys = new ex::SystemManager(world->entities, world->events);
	rendSys->add<RectDrawSystem>(inf);
	rendSys->configure();
}

void TestBounceScene::Update(float dt) {
	world->update(dt);
}

void TestBounceScene::Render() {
	rendSys->update_all(0);
}

TestBounceScene::~TestBounceScene() {
	delete world;
	delete rendSys;
}