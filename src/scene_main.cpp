#include "scene_main.h"
#include <nanovg.h>
#include <random>

#include "sys/systems.h"

std::random_device rand_dev;
std::mt19937 g(rand_dev());
std::uniform_real_distribution<double> distr(0, 1);

void MainScene::Startup(ClientInfo* info) {
	inf = info;

	for (int i = 0; i < 16; i++) {
		double x = (i % 4) * (inf->width / 4);
		double y = floor(i / 4) * (inf->height / 4);
		double w = distr(g) * 50 + 40;
		double h = distr(g) * 50 + 40;
		double dx = distr(g) * 200 * (x < 2 ? 1 : -1) + 50;
		double dy = distr(g) * 200 * (y < 2 ? 1 : -1) + 50;

		auto ent = es.create();
		ent.assign<Body>(x, y, w, h);
		ent.assign<Movable>(dx, dy);
		ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);
	}

	updateSystems.push_back(new RectMoverSystem());

	renderSystems.push_back(new RectDrawSystem());
}

void MainScene::Update(double dt) {
	for (auto sys : updateSystems) {
		sys->update(es, dt);
	}
}

void MainScene::Render() {
	for (auto sys : renderSystems) {
		sys->render(es, inf);
	}
}

void MainScene::Teardown() {

}