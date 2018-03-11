#include "scene_wren.h"
#include "public.h"
#include "wrenapi.h"

void WrenScene::Startup(ClientInfo* info) {
	inf = info;
	vm = Wren_Init();
}

void WrenScene::Update(float dt) {
	Wren_Update(vm, dt);
}

void WrenScene::Render() {
	Wren_Draw(vm, inf->width, inf->height);
}

WrenScene::~WrenScene()
{
	// FIXME: lots of wren stuff
}