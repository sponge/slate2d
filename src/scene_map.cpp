#include "scene_map.h"
#include <nanovg.h>
#include <random>
#include <tmx.h>

#include "sys/systems.h"

void MapScene::Startup(ClientInfo* info) {
	inf = info;
	Entity ent;

	auto m = tmx_load("base/maps/smw.tmx");
	if (!m) tmx_perror("error");

	tmx_map_free(m);

	updateSystems.push_back(new RectMoverSystem());

	renderSystems.push_back(new RectDrawSystem());
}

void MapScene::Update(double dt) {
	for (auto sys : updateSystems) {
		sys->update(es, dt);
	}
}

void MapScene::Render() {
	for (auto sys : renderSystems) {
		sys->render(es, inf);
	}
}

void MapScene::Teardown() {

}