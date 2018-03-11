#include "wrenapi.h"
#include "public.h"
#include "game.h"
#include "draw.h"
#include <cstring>
#include "wren/wren.hpp"

#pragma region Native Functions

// trap module

void trap_print(WrenVM *lvm) {
	const char *str = wrenGetSlotString(lvm, 1);
	trap->Print("%s", str);
}

void trap_console(WrenVM *lvm) {
	const char *str = wrenGetSlotString(lvm, 1);
	trap->SendConsoleCommand(str);
}

void trap_error(WrenVM *lvm) {
	int err = (int) wrenGetSlotDouble(lvm, 1);
	const char *str = wrenGetSlotString(lvm, 2);
	trap->Error(err, va("%s",str));
}

void trap_asset_create(WrenVM *lvm) {
	AssetType_t assetType = (AssetType_t)(int)wrenGetSlotDouble(lvm, 1);
	const char *name = wrenGetSlotString(lvm, 2);
	const char *path = wrenGetSlotString(lvm, 3);

	wrenSetSlotDouble(lvm, 0, trap->Asset_Create(assetType, name, path));
}

void trap_asset_find(WrenVM *lvm) {
	const char *name = wrenGetSlotString(lvm, 1);
	wrenSetSlotDouble(lvm, 0, trap->Asset_Find(name));
}

void trap_asset_loadall(WrenVM *lvm) {
	trap->Asset_LoadAll();
}

void trap_asset_clearall(WrenVM *lvm) {
	trap->Asset_ClearAll();
}

void trap_asset_bmpfnt_set(WrenVM *lvm) {
	AssetHandle assetHandle = (AssetHandle)(int)wrenGetSlotDouble(lvm, 1);
	const char *glyphs = wrenGetSlotString(lvm, 2);
	int charSpacing = (int)wrenGetSlotDouble(lvm, 3);
	int intWidth = (int)wrenGetSlotDouble(lvm, 4);
	int lineHeight = (int)wrenGetSlotDouble(lvm, 5);

	trap->Asset_BMPFNT_Set(assetHandle, glyphs, charSpacing, intWidth, lineHeight);
}

void trap_snd_play(WrenVM *lvm) {
	AssetHandle assetHandle = (AssetHandle)(int)wrenGetSlotDouble(lvm, 1);
	float volume = (float) wrenGetSlotDouble(lvm, 2);
	float pan = (float) wrenGetSlotDouble(lvm, 3);
	bool loop = (float) wrenGetSlotBool(lvm, 4);

	trap->Snd_Play(assetHandle, volume, pan, loop);
}

void trap_create_sprite(WrenVM *lvm) {
	AssetHandle assetHandle = (AssetHandle)(int)wrenGetSlotDouble(lvm, 1);
	int width = (int)wrenGetSlotDouble(lvm, 2);
	int height = (int)wrenGetSlotDouble(lvm, 3);
	int marginX = (int)wrenGetSlotDouble(lvm, 4);
	int marginY = (int)wrenGetSlotDouble(lvm, 5);

	Sprite spr = DC_CreateSprite(assetHandle, width, height, marginX, marginY);
	wrenSetSlotBytes(lvm, 0, (const char*)&spr, sizeof(spr));
}

// draw module

void wren_dc_setcolor(WrenVM *lvm) {
	double which = wrenGetSlotDouble(lvm, 1);
	byte r = (byte) wrenGetSlotDouble(lvm, 2);
	byte g = (byte) wrenGetSlotDouble(lvm, 3);
	byte b = (byte) wrenGetSlotDouble(lvm, 4);
	byte a = (byte) wrenGetSlotDouble(lvm, 5);
	DC_SetColor(which != 0.0f ? OUTLINE : FILL, r, g, b, a);
}

void wren_dc_settransform(WrenVM *lvm) {
	bool absolute = wrenGetSlotBool(lvm, 1);
	byte a = (byte) wrenGetSlotDouble(lvm, 2);
	byte b = (byte) wrenGetSlotDouble(lvm, 3);
	byte c = (byte) wrenGetSlotDouble(lvm, 4);
	byte d = (byte) wrenGetSlotDouble(lvm, 5);
	byte e = (byte) wrenGetSlotDouble(lvm, 6);
	byte f = (byte) wrenGetSlotDouble(lvm, 7);

	DC_SetTransform(absolute, a, b, c, d, e, f);
}

void wren_dc_setscissor(WrenVM *lvm) {
	float x = (float) wrenGetSlotDouble(lvm, 1);
	float y = (float) wrenGetSlotDouble(lvm, 2);
	float w = (float) wrenGetSlotDouble(lvm, 3);
	float h = (float) wrenGetSlotDouble(lvm, 4);

	DC_SetScissor(x, y, w, h);
}

void wren_dc_resetscissor(WrenVM *lvm) {
	DC_ResetScissor();
}

void wren_dc_drawrect(WrenVM *lvm) {
	float x = (float) wrenGetSlotDouble(lvm, 1);
	float y = (float) wrenGetSlotDouble(lvm, 2);
	float w = (float) wrenGetSlotDouble(lvm, 3);
	float h = (float) wrenGetSlotDouble(lvm, 4);
	bool outline = wrenGetSlotBool(lvm, 5);

	DC_DrawRect(x, y, w, h, outline);
}

void wren_dc_drawtext(WrenVM *lvm) {

}

void wren_dc_drawbmptext(WrenVM *lvm) {

}

void wren_dc_drawimage(WrenVM *lvm) {

}

void wren_dc_drawline(WrenVM *lvm) {

}

void wren_dc_drawcircle(WrenVM *lvm) {

}

void wren_dc_drawtri(WrenVM *lvm) {

}

void wren_dc_drawmaplayer(WrenVM *lvm) {

}

void wren_dc_drawsprite(WrenVM *lvm) {
	int sz = 0;
	Sprite *sprite = (Sprite*)wrenGetSlotBytes(lvm, 1, &sz);
	int id = (int)wrenGetSlotDouble(lvm, 2);
	float x = (float)wrenGetSlotDouble(lvm, 3);
	float y = (float)wrenGetSlotDouble(lvm, 4);
	float alpha = (float)wrenGetSlotDouble(lvm, 5);
	float flipBits = (float)wrenGetSlotDouble(lvm, 6);
	float w = (float)wrenGetSlotDouble(lvm, 7);
	float h = (float)wrenGetSlotDouble(lvm, 8);

	DC_DrawSprite(*sprite, id, x, y, alpha, flipBits, w, h);
}

void wren_dc_submit(WrenVM *lvm) {
	DC_Submit();
}

void wren_dc_clear(WrenVM *lvm) {
	DC_Clear();
}

#pragma endregion

static void wren_error(WrenVM* lvm, WrenErrorType type,	const char* module, int line, const char* message) {
	trap->Print("%s:%i - %s\n", module, line, message);
}

char* wren_loadModuleFn(WrenVM* lvm, const char* name) {
	static char *script;
	if (script != nullptr) {
		free(script);
	}

	const char *path = va("scripts/%s.wren", name);
	int sz = trap->FS_ReadFile(path, (void**)&script);
	if (sz <= 0) {
		return nullptr;
	}
	else {
		return script; // FIXME: leak? how do i free script
	}
}

typedef struct {
	const char *module;
	const char *className;
	bool isStatic;
	const char *signature;
	WrenForeignMethodFn fn;
} wrenMethodDef;

static const wrenMethodDef methods[] = {
	{ "engine", "Trap", true, "print(_)", trap_print },
	{ "engine", "Trap", true, "console(_)", trap_console },
	{ "engine", "Trap", true, "sndPlay(_,_,_,_)", trap_snd_play },

	{ "engine", "Asset", true, "create(_,_,_)", trap_asset_create },
	{ "engine", "Asset", true, "find(_)", trap_asset_find },
	{ "engine", "Asset", true, "loadAll()", trap_asset_loadall },
	{ "engine", "Asset", true, "clearAll()", trap_asset_clearall },
	{ "engine", "Asset", true, "bmpfntSet(_,_,_,_,_)", trap_asset_bmpfnt_set },
	{ "engine", "Asset", true, "createSprite(_,_,_,_,_)", trap_create_sprite },

	{ "engine", "Draw", true, "setColor(_,_,_,_,_)", wren_dc_setcolor },
	{ "engine", "Draw", true, "setTransform(_,_,_,_,_,_,_)", wren_dc_settransform },
	{ "engine", "Draw", true, "setScissor(_,_,_,_)", wren_dc_setscissor },
	{ "engine", "Draw", true, "resetScissor()", wren_dc_resetscissor },
	{ "engine", "Draw", true, "rect(_,_,_,_,_)", wren_dc_drawrect },
	{ "engine", "Draw", true, "text(_,_,_,_)", wren_dc_drawtext },
	{ "engine", "Draw", true, "bmpText(_,_,_,_,_)", wren_dc_drawbmptext },
	{ "engine", "Draw", true, "image(_,_,_,_,_,_,_,_,_,_)", wren_dc_drawimage },
	{ "engine", "Draw", true, "line(_,_,_,_)", wren_dc_drawline },
	{ "engine", "Draw", true, "circle(_,_,_,_)", wren_dc_drawcircle },
	{ "engine", "Draw", true, "tri(_,_,_,_,_,_,_)", wren_dc_drawtri },
	{ "engine", "Draw", true, "mapLayer(_,_,_,_,_,_,_)", wren_dc_drawmaplayer },
	{ "engine", "Draw", true, "sprite(_,_,_,_,_,_,_,_)", wren_dc_drawsprite },
	{ "engine", "Draw", true, "submit()", wren_dc_submit },
	{ "engine", "Draw", true, "clear()", wren_dc_clear },
};
static const int methodsCount = sizeof(methods) / sizeof(wrenMethodDef);

WrenForeignMethodFn wren_bindForeignMethodFn(WrenVM* lvm, const char* module, const char* className, bool isStatic, const char* signature) {
	for (int i = 0; i < methodsCount; i++) {
		const wrenMethodDef &m = methods[i];
		if (strcmp(module, m.module) == 0 && strcmp(className, m.className) == 0 && isStatic == m.isStatic && strcmp(signature, m.signature) == 0) {
			return m.fn;
		}
	}

	return nullptr;
}

WrenVM *Wren_Init() {
	WrenConfiguration config;
	wrenInitConfiguration(&config);
	config.errorFn = wren_error;
	config.bindForeignMethodFn = wren_bindForeignMethodFn;
	config.loadModuleFn = wren_loadModuleFn;

	WrenVM *vm = wrenNewVM(&config);

	// load scripts/main.wren
	char *mainStr;
	int mainSz = trap->FS_ReadFile("scripts/main.wren", (void**)&mainStr);
	if (mainSz <= 0) {
		trap->Error(ERR_FATAL, "couldn't load scripts/main.wren");
		return nullptr;
	}

	if (wrenInterpret(vm, mainStr) != WREN_RESULT_SUCCESS) {
		trap->Error(ERR_FATAL, "can't compile scripts/main.wren");
		return nullptr;
	}
	free(mainStr);

	// make sure we can find a new Game class
	wrenEnsureSlots(vm, 1);
	wrenGetVariable(vm, "main", "Game", 0);
	WrenHandle *game_class = wrenGetSlotHandle(vm, 0);

	if (game_class == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find Game class");
		return nullptr;
	}

	wrenHandles_t *hnd = new wrenHandles_t();

	// make a new instance of the Game class and grab handles to update/draw
	WrenHandle *newHnd = wrenMakeCallHandle(vm, "new()");
	hnd->updateHnd = wrenMakeCallHandle(vm, "update(_)");
	hnd->drawHnd = wrenMakeCallHandle(vm, "draw(_,_)");

	if (hnd->updateHnd == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find update(_) on Game class (did you subclass Scene?)");
		return nullptr;
	}

	if (hnd->drawHnd == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find draw(_,_) on Game class (did you subclass Scene?)");
		return nullptr;
	}

	// instantiate a new Game
	wrenSetSlotHandle(vm, 0, game_class);
	wrenCall(vm, newHnd);
	wrenReleaseHandle(vm, newHnd);

	if (wrenGetSlotCount(vm) == 0) {
		trap->Error(ERR_FATAL, "couldn't instantiate new Game class");
		return nullptr;
	}

	hnd->instanceHnd = wrenGetSlotHandle(vm, 0);

	wrenSetUserData(vm, hnd);

	return vm;
}

void Wren_Frame(WrenVM *vm, float dt, int w, int h) {
	wrenHandles_t* hnd = (wrenHandles_t*) wrenGetUserData(vm);
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, hnd->instanceHnd);
	wrenSetSlotDouble(vm, 1, dt);
	wrenCall(vm, hnd->updateHnd);

	wrenEnsureSlots(vm, 3);
	wrenSetSlotHandle(vm, 0, hnd->instanceHnd);
	wrenSetSlotDouble(vm, 1, w);
	wrenSetSlotDouble(vm, 2, h);
	wrenCall(vm, hnd->drawHnd);
}
