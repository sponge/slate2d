#include "scene_main.h"
#include <nanovg.h>
#include <random>

std::random_device rand_dev;
std::mt19937 g(rand_dev());
std::uniform_real_distribution<double> distr(0, 1);

void MainScene::Startup(ClientInfo* info) {
	inf = info;

	for (int i = 0; i < 16; i++) {
		int x = (i % 4) * (inf->width / 4);
		int y = floor(i / 4) * (inf->height / 4);
		int w = distr(g) * 50 + 40;
		int h = distr(g) * 50 + 40;
		double dx = distr(g) * 200 * (x < 2 ? 1 : -1) + 50;
		double dy = distr(g) * 200 * (y < 2 ? 1 : -1) + 50;

		auto ent = es.create();
		ent.assign<Body>(x, y, w, h);
		ent.assign<Movable>(dx, dy);
		ent.assign<Renderable>(distr(g) * 255, distr(g) * 255, distr(g) * 255, distr(g) * 200 + 55);
	}
}

void MainScene::Update(double dt) {
/*	x += 300*dt;
	y += 300*dt;

	int w = i->width, h = i->height;

	if (y > h) {
		y = 0;
	}

	if (x > w) {
		x = 0;
	}*/


	for (auto ent : es.entities_with_components<Body, Movable, Renderable>()) {
		auto body = ent.component<Body>();
		auto movable = ent.component<Movable>();

		body->x += (movable->dx * dt);
		body->y += (movable->dy * dt);

		body->x = body->x > inf->width ? 0 : body->x < -body->w ? inf->width : body->x;
		body->y = body->h > inf->height ? 0 : body->y < -body->h ? inf->height: body->y;
	}
}

void MainScene::Render() {
	NVGcontext *nvg = inf->nvg;

	for (auto ent : es.entities_with_components<Body, Movable, Renderable>()) {
		auto body = ent.component<Body>();
		auto r = ent.component<Renderable>();

		nvgBeginPath(nvg);
		nvgRect(nvg, body->x, body->y, body->w, body->h);
		nvgFillColor(nvg, nvgRGBA(r->r, r->g, r->b, r->a));
		nvgFill(nvg);
	}
}

void MainScene::Teardown() {

}