#include "wrenapi.h"
#include "public.h"
#include "wren/wren.hpp"
#include <cstring>

WrenVM *vm;
WrenHandle *instanceHnd, *updateHnd, *drawHnd;

#pragma region Native Functions

void trap_print(WrenVM *lvm) {
	const char *str = wrenGetSlotString(lvm, 1);
	trap->Print("%s", str);
}

void trap_console(WrenVM *lvm) {
	const char *str = wrenGetSlotString(lvm, 1);
	trap->SendConsoleCommand(str);
}

#pragma endregion

static void wren_error(WrenVM* lvm, WrenErrorType type,	const char* module, int line, const char* message) {
	trap->Print("%s:%i - %s\n", module, line, message);
}

char* wrenLoadModuleFn(WrenVM* lvm, const char* name) {
	return nullptr;
}

typedef struct {
	const char *module;
	const char *className;
	bool isStatic;
	const char *signature;
	WrenForeignMethodFn fn;
} wrenMethodDef;

static const wrenMethodDef methods[] = {
	{ "main", "Trap", true, "print(_)", trap_print },
	{ "main", "Trap", true, "console(_)", trap_console},
};
static const int methodsCount = sizeof(methods) / sizeof(wrenMethodDef);

WrenForeignMethodFn bindForeignMethodFn(WrenVM* lvm, const char* module, const char* className, bool isStatic, const char* signature) {
	for (int i = 0; i < methodsCount; i++) {
		const wrenMethodDef &m = methods[i];
		if (strcmp(module, m.module) == 0 && strcmp(className, m.className) == 0 && isStatic == m.isStatic && strcmp(signature, m.signature) == 0) {
			return m.fn;
		}
	}

	return nullptr;
}

static const char *wren_api = "\n\
class Trap { \n\
	foreign static print(text) \n\
	foreign static console(text) \n\
} \n\
\n\
class Scene { \n\
	construct new() {} \n\
	update(dt) {} \n\
	draw() {} \n\
} \n\
";

void Wren_Init() {
	WrenConfiguration config;
	wrenInitConfiguration(&config);
	config.errorFn = wren_error;
	config.bindForeignMethodFn = bindForeignMethodFn;

	vm = wrenNewVM(&config);

	// compile our internal api and bind all the native functions
	if (wrenInterpret(vm, wren_api) != WREN_RESULT_SUCCESS) {
		trap->Error(ERR_FATAL, "can't load tic api");
	}

	// load scripts/main.wren
	char *mainStr;
	int mainSz = trap->FS_ReadFile("scripts/main.wren", (void**)&mainStr);
	if (mainSz <= 0) {
		trap->Error(ERR_FATAL, "couldn't load scripts/main.wren");
	}

	if (wrenInterpret(vm, mainStr) != WREN_RESULT_SUCCESS) {
		trap->Error(ERR_FATAL, "can't compile scripts/main.wren");
	}
	free(mainStr);

	// make sure we can find a new Game class
	wrenEnsureSlots(vm, 1);
	wrenGetVariable(vm, "main", "Game", 0);
	WrenHandle *game_class = wrenGetSlotHandle(vm, 0);

	if (game_class == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find Game class");
		return;
	}

	// make a new instance of the Game class and grab handles to update/draw
	WrenHandle *newHnd = wrenMakeCallHandle(vm, "new()");
	updateHnd = wrenMakeCallHandle(vm, "update(_)");
	drawHnd = wrenMakeCallHandle(vm, "draw()");

	if (updateHnd == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find update(_) on Game class (did you subclass Scene?)");
		return;
	}

	if (drawHnd == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find draw() on Game class (did you subclass Scene?)");
		return;
	}

	// instantiate a new Game
	wrenSetSlotHandle(vm, 0, game_class);
	wrenCall(vm, newHnd);
	wrenReleaseHandle(vm, newHnd);

	if (wrenGetSlotCount(vm) == 0) {
		trap->Error(ERR_FATAL, "couldn't instantiate new Game class");
		return;
	}

	instanceHnd = wrenGetSlotHandle(vm, 0);
}

void Wren_Frame(float dt) {
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, instanceHnd);
	wrenSetSlotDouble(vm, 1, dt);
	wrenCall(vm, updateHnd);

	wrenEnsureSlots(vm, 1);
	wrenSetSlotHandle(vm, 0, instanceHnd);
	wrenCall(vm, drawHnd);
}
