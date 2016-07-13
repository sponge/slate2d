#include "scene_perf.h"
#include <nanovg.h>

void PerfScene::Startup(ClientInfo* info) {
	i = info;
	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");

	nvgCreateFont(i->nvg, "sans", "base/fonts/Roboto-Regular.ttf");
	nvgCreateFont(i->nvg, "sans-bold", "base/fonts/Roboto-Bold.ttf");
	nvgCreateFont(i->nvg, "sans-light", "base/fonts/Roboto-Light.ttf");
}

void PerfScene::Update(double dt) {
	updateGraph(&fps, dt);
}

void PerfScene::Render() {
	NVGcontext *nvg = i->nvg;

	renderGraph(nvg, 5, 5, &fps);
}

void PerfScene::Teardown() {

}