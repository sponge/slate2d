#include "wrenapi.h"
#include "public.h"
#include "game.h"
#include "draw.h"
#include <cstring>
#include "wren/wren.hpp"

#pragma region Native Functions

static tmx_map *map; // FIXME: bad!

// trap module

void wren_trap_print(WrenVM *vm) {
	const char *str = wrenGetSlotString(vm, 1);
	trap->Print("%s", str);
}

void wren_trap_console(WrenVM *vm) {
	const char *str = wrenGetSlotString(vm, 1);
	trap->SendConsoleCommand(str);
}

void wren_trap_error(WrenVM *vm) {
	int err = (int) wrenGetSlotDouble(vm, 1);
	const char *str = wrenGetSlotString(vm, 2);
	trap->Error(err, va("%s",str));
}

void wren_trap_map_load(WrenVM *vm) {
	const char *str = wrenGetSlotString(vm, 1);

	map = trap->Map_Load(str);
}

void wren_trap_map_free(WrenVM *vm) {
	trap->Map_Free(map);
}

void wren_trap_asset_create(WrenVM *vm) {
	AssetType_t assetType = (AssetType_t)(int)wrenGetSlotDouble(vm, 1);
	const char *name = wrenGetSlotString(vm, 2);
	const char *path = wrenGetSlotString(vm, 3);

	wrenSetSlotDouble(vm, 0, trap->Asset_Create(assetType, name, path));
}

void wren_trap_asset_find(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);
	wrenSetSlotDouble(vm, 0, trap->Asset_Find(name));
}

void wren_trap_asset_loadall(WrenVM *vm) {
	trap->Asset_LoadAll();
}

void wren_trap_asset_clearall(WrenVM *vm) {
	trap->Asset_ClearAll();
}

void wren_trap_asset_bmpfnt_set(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	const char *glyphs = wrenGetSlotString(vm, 2);
	int charSpacing = (int)wrenGetSlotDouble(vm, 3);
	int intWidth = (int)wrenGetSlotDouble(vm, 4);
	int lineHeight = (int)wrenGetSlotDouble(vm, 5);

	trap->Asset_BMPFNT_Set(assetHandle, glyphs, charSpacing, intWidth, lineHeight);
}

void wren_trap_snd_play(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	float volume = (float) wrenGetSlotDouble(vm, 2);
	float pan = (float) wrenGetSlotDouble(vm, 3);
	bool loop = (float) wrenGetSlotBool(vm, 4);

	trap->Snd_Play(assetHandle, volume, pan, loop);
}

void wren_trap_create_sprite(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	int width = (int)wrenGetSlotDouble(vm, 2);
	int height = (int)wrenGetSlotDouble(vm, 3);
	int marginX = (int)wrenGetSlotDouble(vm, 4);
	int marginY = (int)wrenGetSlotDouble(vm, 5);

	Sprite spr = DC_CreateSprite(assetHandle, width, height, marginX, marginY);
	wrenSetSlotBytes(vm, 0, (const char*)&spr, sizeof(spr));
}

// draw module

void wren_dc_setcolor(WrenVM *vm) {
	double which = wrenGetSlotDouble(vm, 1);
	byte r = (byte) wrenGetSlotDouble(vm, 2);
	byte g = (byte) wrenGetSlotDouble(vm, 3);
	byte b = (byte) wrenGetSlotDouble(vm, 4);
	byte a = (byte) wrenGetSlotDouble(vm, 5);
	DC_SetColor(which != 0.0f ? OUTLINE : FILL, r, g, b, a);
}

void wren_dc_settransform(WrenVM *vm) {
	byte a = (byte) wrenGetSlotDouble(vm, 1);
	byte b = (byte) wrenGetSlotDouble(vm, 2);
	byte c = (byte) wrenGetSlotDouble(vm, 3);
	byte d = (byte) wrenGetSlotDouble(vm, 4);
	byte e = (byte) wrenGetSlotDouble(vm, 5);
	byte f = (byte) wrenGetSlotDouble(vm, 6);
	bool absolute = wrenGetSlotBool(vm, 7);

	DC_SetTransform(a, b, c, d, e, f, absolute);
}

void wren_dc_setscissor(WrenVM *vm) {
	float x = (float) wrenGetSlotDouble(vm, 1);
	float y = (float) wrenGetSlotDouble(vm, 2);
	float w = (float) wrenGetSlotDouble(vm, 3);
	float h = (float) wrenGetSlotDouble(vm, 4);

	DC_SetScissor(x, y, w, h);
}

void wren_dc_resetscissor(WrenVM *vm) {
	DC_ResetScissor();
}

void wren_dc_drawrect(WrenVM *vm) {
	float x = (float) wrenGetSlotDouble(vm, 1);
	float y = (float) wrenGetSlotDouble(vm, 2);
	float w = (float) wrenGetSlotDouble(vm, 3);
	float h = (float) wrenGetSlotDouble(vm, 4);
	bool outline = wrenGetSlotBool(vm, 5);

	DC_DrawRect(x, y, w, h, outline);
}

void wren_dc_drawtext(WrenVM *vm) {
	float x = (float)wrenGetSlotDouble(vm, 1);
	float y = (float)wrenGetSlotDouble(vm, 2);
	const char *text = wrenGetSlotString(vm, 3);
	int align = (int)wrenGetSlotDouble(vm, 4);

	DC_DrawText(x, y, text, align);
}

void wren_dc_drawbmptext(WrenVM *vm) {
	AssetHandle fntId = (AssetHandle)wrenGetSlotDouble(vm, 1);
	float x = (float)wrenGetSlotDouble(vm, 2);
	float y = (float)wrenGetSlotDouble(vm, 3);
	const char *text = wrenGetSlotString(vm, 4);
	float scale = (float)wrenGetSlotDouble(vm, 5);

	DC_DrawBmpText(fntId, x, y, text, scale);
}

void wren_dc_drawimage(WrenVM *vm) {
	AssetHandle imgId = (AssetHandle)wrenGetSlotDouble(vm, 1);
	float x = (float)wrenGetSlotDouble(vm, 2);
	float y = (float)wrenGetSlotDouble(vm, 3);
	float w = (float)wrenGetSlotDouble(vm, 4);
	float h = (float)wrenGetSlotDouble(vm, 5);
	float alpha = (float)wrenGetSlotDouble(vm, 6);
	float scale = (float)wrenGetSlotDouble(vm, 7);
	byte flipBits = (byte)wrenGetSlotDouble(vm, 8);
	float ox = (float)wrenGetSlotDouble(vm, 9);
	float oy = (float)wrenGetSlotDouble(vm, 10);
	int shaderId = (int)wrenGetSlotDouble(vm, 11);

	DC_DrawImage(imgId, x, y, w, h, alpha, scale, flipBits, ox, oy, shaderId);
}

void wren_dc_drawline(WrenVM *vm) {
	float x1 = (float)wrenGetSlotDouble(vm, 1);
	float y1 = (float)wrenGetSlotDouble(vm, 2);
	float x2 = (float)wrenGetSlotDouble(vm, 3);
	float y2 = (float)wrenGetSlotDouble(vm, 4);

	DC_DrawLine(x1, y1, x2, y2);
}

void wren_dc_drawcircle(WrenVM *vm) {
	float x = (float)wrenGetSlotDouble(vm, 1);
	float y = (float)wrenGetSlotDouble(vm, 2);
	float radius = (float)wrenGetSlotDouble(vm, 3);
	bool outline = wrenGetSlotBool(vm, 4);

	DC_DrawCircle(x, y, radius, outline);
}

void wren_dc_drawtri(WrenVM *vm) {
	float x1 = (float)wrenGetSlotDouble(vm, 1);
	float y1 = (float)wrenGetSlotDouble(vm, 2);
	float x2 = (float)wrenGetSlotDouble(vm, 3);
	float y2 = (float)wrenGetSlotDouble(vm, 4);
	float x3 = (float)wrenGetSlotDouble(vm, 5);
	float y3 = (float)wrenGetSlotDouble(vm, 6);
	bool outline = wrenGetSlotBool(vm, 7);

	DC_DrawTri(x1, y1, x2, y2, x3, y3, outline);
}

void wren_dc_drawmaplayer(WrenVM *vm) {
	int layer = (int)wrenGetSlotDouble(vm, 1);
	float x = (float)wrenGetSlotDouble(vm, 2);
	float y = (float)wrenGetSlotDouble(vm, 3);
	unsigned int cellX = (unsigned int)wrenGetSlotDouble(vm, 4);
	unsigned int cellY = (unsigned int)wrenGetSlotDouble(vm, 5);
	unsigned int cellW = (unsigned int)wrenGetSlotDouble(vm, 6);
	unsigned int cellH = (unsigned int)wrenGetSlotDouble(vm, 7);

	DC_DrawMapLayer(layer, x, y, cellX, cellY, cellW, cellH);
}

void wren_dc_drawsprite(WrenVM *vm) {
	int sz = 0;
	Sprite *sprite = (Sprite*)wrenGetSlotBytes(vm, 1, &sz);
	int id = (int)wrenGetSlotDouble(vm, 2);
	float x = (float)wrenGetSlotDouble(vm, 3);
	float y = (float)wrenGetSlotDouble(vm, 4);
	float alpha = (float)wrenGetSlotDouble(vm, 5);
	float scale = (float)wrenGetSlotDouble(vm, 6);
	byte flipBits = (byte)wrenGetSlotDouble(vm, 7);
	int w = (int)wrenGetSlotDouble(vm, 8);
	int h = (int)wrenGetSlotDouble(vm, 9);

	DC_DrawSprite(*sprite, id, x, y, alpha, scale, flipBits, w, h);
}

void wren_dc_submit(WrenVM *vm) {
	DC_Submit();
}

void wren_dc_clear(WrenVM *vm) {
	DC_Clear();
}

#pragma endregion

static void wren_error(WrenVM* vm, WrenErrorType type,	const char* module, int line, const char* message) {
	trap->Print("%s:%i - %s\n", module, line, message);
}

char* wren_loadModuleFn(WrenVM* vm, const char* name) {
	static char *script;
	if (script != nullptr) {
		//free(script);
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
	{ "engine", "Trap", true, "print(_)", wren_trap_print },
	{ "engine", "Trap", true, "console(_)", wren_trap_console },
	{ "engine", "Trap", true, "sndPlay(_,_,_,_)", wren_trap_snd_play },
	{ "engine", "Trap", true, "mapLoad(_)", wren_trap_map_load },
	{ "engine", "Trap", true, "mapFree()", wren_trap_map_free },


	{ "engine", "Asset", true, "create(_,_,_)", wren_trap_asset_create },
	{ "engine", "Asset", true, "find(_)", wren_trap_asset_find },
	{ "engine", "Asset", true, "loadAll()", wren_trap_asset_loadall },
	{ "engine", "Asset", true, "clearAll()", wren_trap_asset_clearall },
	{ "engine", "Asset", true, "bmpfntSet(_,_,_,_,_)", wren_trap_asset_bmpfnt_set },
	{ "engine", "Asset", true, "createSprite(_,_,_,_,_)", wren_trap_create_sprite },

	{ "engine", "Draw", true, "setColor(_,_,_,_,_)", wren_dc_setcolor },
	{ "engine", "Draw", true, "setTransform(_,_,_,_,_,_,_)", wren_dc_settransform },
	{ "engine", "Draw", true, "setScissor(_,_,_,_)", wren_dc_setscissor },
	{ "engine", "Draw", true, "resetScissor()", wren_dc_resetscissor },
	{ "engine", "Draw", true, "rect(_,_,_,_,_)", wren_dc_drawrect },
	{ "engine", "Draw", true, "text(_,_,_,_)", wren_dc_drawtext },
	{ "engine", "Draw", true, "bmpText(_,_,_,_,_)", wren_dc_drawbmptext },
	{ "engine", "Draw", true, "image(_,_,_,_,_,_,_,_,_,_,_)", wren_dc_drawimage },
	{ "engine", "Draw", true, "line(_,_,_,_)", wren_dc_drawline },
	{ "engine", "Draw", true, "circle(_,_,_,_)", wren_dc_drawcircle },
	{ "engine", "Draw", true, "tri(_,_,_,_,_,_,_)", wren_dc_drawtri },
	{ "engine", "Draw", true, "mapLayer(_,_,_,_,_,_,_)", wren_dc_drawmaplayer },
	{ "engine", "Draw", true, "sprite(_,_,_,_,_,_,_,_,_)", wren_dc_drawsprite },
	{ "engine", "Draw", true, "submit()", wren_dc_submit },
	{ "engine", "Draw", true, "clear()", wren_dc_clear },
};
static const int methodsCount = sizeof(methods) / sizeof(wrenMethodDef);

WrenForeignMethodFn wren_bindForeignMethodFn(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature) {
	for (int i = 0; i < methodsCount; i++) {
		const wrenMethodDef &m = methods[i];
		if (strcmp(module, m.module) == 0 && strcmp(className, m.className) == 0 && isStatic == m.isStatic && strcmp(signature, m.signature) == 0) {
			return m.fn;
		}
	}

	return nullptr;
}

WrenVM *Wren_Init(const char *constructorStr) {
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
	WrenHandle *newHnd = wrenMakeCallHandle(vm, "new(_)");
	hnd->updateHnd = wrenMakeCallHandle(vm, "update(_)");
	hnd->drawHnd = wrenMakeCallHandle(vm, "draw(_,_)");
	hnd->shutdownHnd = wrenMakeCallHandle(vm, "shutdown()");

	if (hnd->updateHnd == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find update(_) on Game class (did you subclass Scene?)");
		return nullptr;
	}

	if (hnd->drawHnd == nullptr) {
		trap->Error(ERR_FATAL, "couldn't find draw(_,_) on Game class (did you subclass Scene?)");
		return nullptr;
	}

	// instantiate a new Game
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, game_class);
	wrenSetSlotString(vm, 1, constructorStr);
	wrenCall(vm, newHnd);
	wrenReleaseHandle(vm, newHnd);
	wrenReleaseHandle(vm, game_class);

	if (wrenGetSlotCount(vm) == 0) {
		trap->Error(ERR_FATAL, "couldn't instantiate new Game class");
		return nullptr;
	}

	hnd->instanceHnd = wrenGetSlotHandle(vm, 0);

	wrenSetUserData(vm, hnd);

	return vm;
}

void Wren_Update(WrenVM *vm, float dt) {
	wrenHandles_t* hnd = (wrenHandles_t*)wrenGetUserData(vm);
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, hnd->instanceHnd);
	wrenSetSlotDouble(vm, 1, dt);
	wrenCall(vm, hnd->updateHnd);
}

void Wren_Draw(WrenVM *vm, int w, int h) {
	wrenHandles_t* hnd = (wrenHandles_t*)wrenGetUserData(vm);
	wrenEnsureSlots(vm, 3);
	wrenSetSlotHandle(vm, 0, hnd->instanceHnd);
	wrenSetSlotDouble(vm, 1, w);
	wrenSetSlotDouble(vm, 2, h);
	wrenCall(vm, hnd->drawHnd);
}

void Wren_Scene_Shutdown(WrenVM *vm) {
	wrenHandles_t* hnd = (wrenHandles_t*)wrenGetUserData(vm);
	wrenEnsureSlots(vm, 1);
	wrenSetSlotHandle(vm, 0, hnd->instanceHnd);
	wrenCall(vm, hnd->shutdownHnd);
}

void Wren_FreeVM(WrenVM *vm) {
	wrenHandles_t* hnd = (wrenHandles_t*)wrenGetUserData(vm);

	wrenReleaseHandle(vm, hnd->drawHnd);
	wrenReleaseHandle(vm, hnd->updateHnd);
	wrenReleaseHandle(vm, hnd->shutdownHnd);
	wrenReleaseHandle(vm, hnd->instanceHnd);
	free(hnd);

	wrenFreeVM(vm);
}
