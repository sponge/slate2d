#include "scene_test.h"
#include <nanovg.h>
#include <random>
#include <tmx.h>

#include "sys/systems.h"

void TestScene::Startup(ClientInfo* info) {
	inf = info;
	Entity ent;

	std::random_device rand_dev;
	std::mt19937 g(rand_dev());
	std::uniform_real_distribution<double> distr(0, 1);

	// rolling into platform
	ent = es.create();
	ent.assign<Body>(50, 100, 50, 50);
	ent.assign<Movable>(60, 500);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	ent = es.create();
	ent.assign<Body>(300, 500, 500, 50);
	ent.assign<Movable>(0, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	// two way diag collision
	ent = es.create();
	ent.assign<Body>(100, 100, 50, 50);
	ent.assign<Movable>(20, 20);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	ent = es.create();
	ent.assign<Body>(200, 200, 50, 50);
	ent.assign<Movable>(0, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	// horiz scrape by
	ent = es.create();
	ent.assign<Body>(400, 100, 50, 50);
	ent.assign<Movable>(20, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	ent = es.create();
	ent.assign<Body>(475, 150, 50, 50);
	ent.assign<Movable>(-20, 0);
	ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);

	updateSystems.push_back(new RectMoverSystem());

	renderSystems.push_back(new RectDrawSystem());
}

void TestScene::Update(double dt) {
	for (auto sys : updateSystems) {
		sys->update(es, dt);
	}
}

void TestScene::Render() {
	for (auto sys : renderSystems) {
		sys->render(es, inf);
	}
}

void TestScene::Teardown() {

}