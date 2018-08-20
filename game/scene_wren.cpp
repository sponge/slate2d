#include "scene_wren.h"
#include "public.h"
#include "wrenapi.h"

void WrenScene::Startup(ClientInfo* info) {
	inf = info;
	vm = Wren_Init(mainScriptName, mapFileName);
}

void WrenScene::Update(double dt) {
	Wren_Update(vm, dt);
}

void WrenScene::Render() {
	Wren_Draw(vm, inf->width, inf->height);
}

WrenScene::~WrenScene()
{
	Wren_Scene_Shutdown(vm);
	Wren_FreeVM(vm);
}

void WrenScene::Console(const char *str)
{
	Wren_Console(vm, str);
}
