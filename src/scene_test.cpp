#include "scene_test.h"
#include <random>
#include <entityx/entityx.h>

#include "sys/systems_drw.h"
#include "sys/systems_upd.h"
#include "components.h"

TestWorld::TestWorld() {
    systems.add<RectMoverSystem>();
	systems.configure();

	ex::Entity ent;

	std::random_device rand_dev;
	std::mt19937 g(rand_dev());
	std::uniform_real_distribution<double> distr(0, 1);

	// rolling into platform
	ent = this->entities.create();
	ent.assign<Body>(50, 100, 50, 50);
	ent.assign<Movable>(60, 500);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	ent = this->entities.create();
	ent.assign<Body>(300, 500, 500, 50);
	ent.assign<Movable>(0, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	// two way diag collision
	ent = this->entities.create();
	ent.assign<Body>(100, 100, 50, 50);
	ent.assign<Movable>(20, 20);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	ent = this->entities.create();
	ent.assign<Body>(200, 200, 50, 50);
	ent.assign<Movable>(0, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	// horiz scrape by
	ent = this->entities.create();
	ent.assign<Body>(400, 100, 50, 50);
	ent.assign<Movable>(20, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	ent = this->entities.create();
	ent.assign<Body>(475, 150, 50, 50);
	ent.assign<Movable>(-20, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

}

void TestWorld::update(ex::TimeDelta dt) {
	systems.update_all(dt);
}

void TestScene::Startup(ClientInfo* info) {
	inf = info;

	world = new TestWorld();

	rendSys = new ex::SystemManager(world->entities, world->events);
	rendSys->add<RectDrawSystem>(inf);
	rendSys->configure();
}

void TestScene::Update(double dt) {
	world->update(dt);
}

void TestScene::Render() {
	rendSys->update_all(0);
}