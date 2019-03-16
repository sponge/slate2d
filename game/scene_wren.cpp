#include "scene_wren.h"
#include "../src/shared.h"
#include "wrenapi.h"

void WrenScene::Startup(ClientInfo* info) {
	inf = info;
	vm = Wren_Init(mainScriptName, mapFileName);
	if (vm != nullptr) {
		initialized = true;
		trap->Con_SetVar("engine.errorMessage", "");
	}
}

bool WrenScene::Update(double dt) {
	return Wren_Update(vm, dt);
}

void WrenScene::Render() {
	Wren_Draw(vm, inf->width, inf->height);
}

WrenScene::~WrenScene() {
	if (!initialized) {
		return;
	}

	Wren_Scene_Shutdown(vm);
	Wren_FreeVM(vm);
}

void WrenScene::Console(const char *str) {
	Wren_Console(vm, str);
}
