#include "wrenapi.h"
#include "public.h"
#include "game.h"
#include "draw.h"
#include <cstring>
#include "wren/wren.hpp"
#include "map.h"
#include <string>
#include <map>
#include <imgui.h>

static tmx_map *map; // FIXME: bad!

#pragma region Trap Module
void wren_trap_print(WrenVM *vm) {
	const char *str = wrenGetSlotString(vm, 1);
	trap->Print("%s", str);
}

void wren_trap_dbgwin(WrenVM *vm) {
	const char *title = wrenGetSlotString(vm, 1);
	const char *key = wrenGetSlotString(vm, 2);
	const char *value = wrenGetSlotString(vm, 3);

	ImGui::Begin(title, nullptr, ImGuiWindowFlags_HorizontalScrollbar);

	float width = ImGui::GetWindowContentRegionWidth();
	float keyWidth = ImGui::CalcTextSize(key).x;
	float valWidth = ImGui::CalcTextSize(value).x;

	ImGui::Text("%s", key);
	if (keyWidth + valWidth + 20 < width) {
		ImGui::SameLine();
	}
	int x = width - valWidth;
	x = x < 5 ? 5 : x;
	ImGui::SetCursorPosX(x);
	ImGui::Text("%s", value);
	ImGui::Separator();
	ImGui::End();
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

void wren_trap_snd_play(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	float volume = (float)wrenGetSlotDouble(vm, 2);
	float pan = (float)wrenGetSlotDouble(vm, 3);
	bool loop = (float)wrenGetSlotBool(vm, 4);

	unsigned int hnd = trap->Snd_Play(assetHandle, volume, pan, loop);
	wrenSetSlotDouble(vm, 0, hnd);
}

void wren_trap_snd_stop(WrenVM *vm) {
	// let's handle this one specifically because otherwise you have to
	// track null yourself. wish i could macro all of this.
	if (wrenGetSlotType(vm, 1) != WREN_TYPE_NUM) {
		return;
	}

	unsigned int handle = (unsigned int)wrenGetSlotDouble(vm, 1);
	trap->Snd_Stop(handle);
}

void wren_trap_in_keystate(WrenVM *vm) {
	int key = (int)wrenGetSlotDouble(vm, 1);

	if (key < 0 || key > MAX_KEYS) {
		wrenSetSlotBool(vm, 0, false);
		return;
	}

	wrenSetSlotBool(vm, 0, trap->IN_KeyState(&buttons[key]) > 0.0f);
}

void wren_trap_in_keypressed(WrenVM *vm) {
	int key = (int)wrenGetSlotDouble(vm, 1);
	int delay = (int)wrenGetSlotDouble(vm, 2);
	int repeat = (int)wrenGetSlotDouble(vm, 3);

	if (key < 0 || key > MAX_KEYS) {
		wrenSetSlotBool(vm, 0, false);
		return;
	}

	wrenSetSlotBool(vm, 0, trap->IN_KeyPressed(&buttons[key], delay, repeat));
}

void wren_trap_mouse_position(WrenVM *vm) {
	MousePosition mousePos = trap->IN_MousePosition();

	wrenEnsureSlots(vm, 3);
	wrenSetSlotNewList(vm, 0);

	wrenSetSlotDouble(vm, 1, mousePos.x);
	wrenSetSlotDouble(vm, 2, mousePos.y);

	wrenInsertInList(vm, 0, 0, 1);
	wrenInsertInList(vm, 0, 0, 2);
}

extern void wren_trap_inspect_instance(WrenVM *vm);
#pragma endregion

#pragma region Asset Module
void wren_asset_create(WrenVM *vm) {
	AssetType_t assetType = (AssetType_t)(int)wrenGetSlotDouble(vm, 1);
	const char *name = wrenGetSlotString(vm, 2);
	const char *path = wrenGetSlotString(vm, 3);

	wrenSetSlotDouble(vm, 0, trap->Asset_Create(assetType, name, path));
}

void wren_asset_find(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);
	wrenSetSlotDouble(vm, 0, trap->Asset_Find(name));
}

void wren_asset_loadall(WrenVM *vm) {
	trap->Asset_LoadAll();
}

void wren_asset_clearall(WrenVM *vm) {
	trap->Asset_ClearAll();
}

void wren_asset_bmpfnt_set(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	const char *glyphs = wrenGetSlotString(vm, 2);
	int glyphWidth = (int)wrenGetSlotDouble(vm, 3);
	int charSpacing = (int)wrenGetSlotDouble(vm, 4);
	int intWidth = (int)wrenGetSlotDouble(vm, 5);
	int lineHeight = (int)wrenGetSlotDouble(vm, 6);

	trap->Asset_BMPFNT_Set(assetHandle, glyphs, glyphWidth, charSpacing, intWidth, lineHeight);
}

void wren_asset_measurebmptext(WrenVM *vm) {
	AssetHandle fntId = (AssetHandle)wrenGetSlotDouble(vm, 1);
	const char *text = wrenGetSlotString(vm, 2);
	float scale = (float)wrenGetSlotDouble(vm, 3);

	double width = trap->Asset_BMPFNT_TextWidth(fntId, text, scale);
	wrenSetSlotDouble(vm, 0, width);
}

void wren_create_sprite(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	int width = (int)wrenGetSlotDouble(vm, 2);
	int height = (int)wrenGetSlotDouble(vm, 3);
	int marginX = (int)wrenGetSlotDouble(vm, 4);
	int marginY = (int)wrenGetSlotDouble(vm, 5);

	Sprite spr = DC_CreateSprite(assetHandle, width, height, marginX, marginY);
	wrenSetSlotBytes(vm, 0, (const char*)&spr, sizeof(spr));
}
#pragma endregion

#pragma region Draw Module
void wren_dc_setcolor(WrenVM *vm) {
	double which = wrenGetSlotDouble(vm, 1);
	byte r = (byte) wrenGetSlotDouble(vm, 2);
	byte g = (byte) wrenGetSlotDouble(vm, 3);
	byte b = (byte) wrenGetSlotDouble(vm, 4);
	byte a = (byte) wrenGetSlotDouble(vm, 5);
	DC_SetColor(which != 0.0f ? OUTLINE : FILL, r, g, b, a);
}

void wren_dc_reset_transform(WrenVM *vm) {
	DC_ResetTransform();
}

void wren_dc_transform(WrenVM *vm) {
	float a = (float) wrenGetSlotDouble(vm, 1);
	float b = (float) wrenGetSlotDouble(vm, 2);
	float c = (float) wrenGetSlotDouble(vm, 3);
	float d = (float) wrenGetSlotDouble(vm, 4);
	float e = (float) wrenGetSlotDouble(vm, 5);
	float f = (float) wrenGetSlotDouble(vm, 6);

	DC_Transform(a, b, c, d, e, f);
}

void wren_dc_rotate(WrenVM *vm) {
	float angle = (float) wrenGetSlotDouble(vm, 1);
	DC_Rotate(angle);
}

void wren_dc_translate(WrenVM *vm) {
	float x = (float) wrenGetSlotDouble(vm, 1);
	float y = (float) wrenGetSlotDouble(vm, 2);
	DC_Translate(x, y);
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

void wren_asset_create_sprite(WrenVM *vm) {
	AssetHandle assetHandle = (AssetHandle)wrenGetSlotDouble(vm, 1);
	int width = (int)wrenGetSlotDouble(vm, 2);
	int height = (int)wrenGetSlotDouble(vm, 3);
	int marginX = (int)wrenGetSlotDouble(vm, 4);
	int marginY = (int)wrenGetSlotDouble(vm, 5);

	Sprite spr = DC_CreateSprite(assetHandle, width, height, marginX, marginY);
	wrenSetSlotBytes(vm, 0, (const char*)&spr, sizeof(spr));
}

void wren_dc_submit(WrenVM *vm) {
	DC_Submit();
}

void wren_dc_clear(WrenVM *vm) {
	DC_Clear();
}
#pragma endregion

#pragma region Map Module

void parseProperties(WrenVM *vm, int propSlot, int *totalSlotsIn, int *sIn, void *properties) {
	int &s = *sIn;
	int &totalSlots = *totalSlotsIn;

	// loop through properties
	if (properties != nullptr) {
		auto &props = *(std::map<std::string, void *>*) properties;

		for (auto item : props) {
			// add 2 slots for key, value
			totalSlots += 2;
			wrenEnsureSlots(vm, totalSlots);

			auto prop = (tmx_property*)item.second;
			wrenSetSlotString(vm, s++, prop->name);
			switch (prop->type) {
			case PT_INT:
				wrenSetSlotDouble(vm, s++, prop->value.integer);
				break;
			case PT_FLOAT:
				wrenSetSlotDouble(vm, s++, prop->value.decimal);
				break;
			case PT_BOOL:
				wrenSetSlotBool(vm, s++, prop->value.boolean);
				break;
			case PT_COLOR:
				wrenSetSlotDouble(vm, s++, prop->value.color);
				break;
			case PT_NONE:
			case PT_STRING:
			case PT_FILE:
			default:
				wrenSetSlotString(vm, s++, prop->value.string);
				break;
			}
			wrenInsertInMap(vm, propSlot, s - 2, s - 1);
		}
	}
}

void wren_map_load(WrenVM *vm) {
	const char *str = wrenGetSlotString(vm, 1);

	map = trap->Map_Load(str);
}

void wren_map_free(WrenVM *vm) {
	trap->Map_Free(map);
}

void wren_map_getlayerbyname(WrenVM *vm) {
	const char *name = wrenGetSlotString(vm, 1);

	int id = Map_GetLayerByName(map, name);

	wrenSetSlotDouble(vm, 0, id);
}

void wren_map_getobjectsinlayer(WrenVM *vm) {
	int id = (int) wrenGetSlotDouble(vm, 1);
	int totalSlots = 1; // total num of slots for wrenEnsureSlots
	int s = 1; // current slot we're on

	static const char *keys[] = { "name", "type", "x", "y", "width", "height", "visible", "rotation", "properties" };
	static const int keySz = sizeof(keys) / sizeof(*keys);

	// reserve keys
	totalSlots += keySz;
	wrenEnsureSlots(vm, totalSlots);

	// return value is a list of map objects
	wrenSetSlotNewList(vm, 0);

	// store the keys starting from slot 1 since they can be reused
	for (int i = 0; i < keySz; i++) {
		wrenSetSlotString(vm, s++, keys[i]);
	}

	tmx_object *obj = Map_LayerObjects(map, id, nullptr);
	while (obj != nullptr) {
		// ensure enough slots for map object + map values
		totalSlots += keySz + 1;
		wrenEnsureSlots(vm, totalSlots);

		// make a new map, push it to the end of the return value list
		int mapSlot = s++;
		wrenSetSlotNewMap(vm, mapSlot);
		wrenInsertInList(vm, 0, -1, mapSlot);

		// slots for property values, same order as keys[]
		wrenSetSlotString(vm, s++, obj->name == nullptr ? "" : obj->name);
		const char *defaultedType = Map_GetObjectType(map, obj);
		wrenSetSlotString(vm, s++, defaultedType == nullptr ? "" : defaultedType);
		wrenSetSlotDouble(vm, s++, obj->x);
		wrenSetSlotDouble(vm, s++, obj->y);
		wrenSetSlotDouble(vm, s++, obj->width);
		wrenSetSlotDouble(vm, s++, obj->height);
		wrenSetSlotBool(vm, s++, obj->visible);
		wrenSetSlotDouble(vm, s++, obj->rotation);

		// properties is a nested map, we'll populate it after we deal with the object
		int propSlot = s++;
		wrenSetSlotNewMap(vm, propSlot);

		// apply the default properties first if it is a tile
		if (obj->obj_type == OT_TILE) {
			tmx_tile *baseTile = Map_GetTileInfo(map, obj->content.gid);
			if (baseTile != nullptr) {
				parseProperties(vm, propSlot, &totalSlots, &s, baseTile->properties);
			}
		}
		// add the object
		parseProperties(vm, propSlot, &totalSlots, &s, obj->properties);

		for (int i = 0; i < keySz; i++) {
			// i + 1 because 0 is return value. keys are at the top, values are placed after the map
			wrenInsertInMap(vm, mapSlot, i + 1, mapSlot + 1 + i);
		}

		obj = Map_LayerObjects(map, id, obj);
	}
}

void wren_map_getmapproperties(WrenVM *vm) {
	static const char *keys[] = { "width", "height", "tileWidth", "tileHeight", "backgroundColor", "properties" };
	static const int keySz = sizeof(keys) / sizeof(*keys);

	int totalSlots = 1; // total num of slots for wrenEnsureSlots
	int s = 1; // current slot we're on

	// reserve keys
	totalSlots += keySz * 2;
	wrenEnsureSlots(vm, totalSlots);

	// return value is a map
	wrenSetSlotNewMap(vm, 0);

	// store the keys starting from slot 1
	for (int i = 0; i < keySz; i++) {
		wrenSetSlotString(vm, s++, keys[i]);
	}

	// values
	wrenSetSlotDouble(vm, s++, map->width);
	wrenSetSlotDouble(vm, s++, map->height);
	wrenSetSlotDouble(vm, s++, map->tile_width);
	wrenSetSlotDouble(vm, s++, map->tile_height);
	wrenSetSlotDouble(vm, s++, map->backgroundcolor);

	int propSlot = s++;
	wrenSetSlotNewMap(vm, propSlot);

	parseProperties(vm, propSlot, &totalSlots, &s, map->properties);

	for (int i = 0; i < keySz; i++) {
		wrenInsertInMap(vm, 0, 1 + i, 1 + keySz + i);
	}
}

void wren_map_getlayerproperties(WrenVM *vm) {
	int id = (int)wrenGetSlotDouble(vm, 1);
	tmx_layer *layer = Map_GetLayer(map, id);

	if (layer == nullptr) {
		return;
	}

	static const char *keys[] = { "name", "visible", "opacity", "offsetX", "offsetY", "properties" };
	static const int keySz = sizeof(keys) / sizeof(*keys);

	int totalSlots = 1; // total num of slots for wrenEnsureSlots
	int s = 1; // current slot we're on

	// reserve keys
	totalSlots += keySz * 2;
	wrenEnsureSlots(vm, totalSlots);

	// return value is a map
	wrenSetSlotNewMap(vm, 0);

	// store the keys starting from slot 1
	for (int i = 0; i < keySz; i++) {
		wrenSetSlotString(vm, s++, keys[i]);
	}

	wrenSetSlotString(vm, s++, layer->name == nullptr ? "" : layer->name);
	wrenSetSlotBool(vm, s++, layer->visible);
	wrenSetSlotDouble(vm, s++, layer->opacity);
	wrenSetSlotDouble(vm, s++, layer->offsetx);
	wrenSetSlotDouble(vm, s++, layer->offsety);
	int propSlot = s++;
	wrenSetSlotNewMap(vm, propSlot);

	parseProperties(vm, propSlot, &totalSlots, &s, layer->properties);

	for (int i = 0; i < keySz; i++) {
		wrenInsertInMap(vm, 0, 1 + i, 1 + keySz + i);
	}
}

void wren_map_gettileproperties(WrenVM *vm) {
	int totalSlots = 2; // total num of slots for wrenEnsureSlots
	int s = 1; // current slot we're on

	// return value is a list of map objects
	wrenSetSlotNewList(vm, 0);

	wrenEnsureSlots(vm, totalSlots);
	wrenSetSlotString(vm, s++, "type");

	for (unsigned int gid = 0; gid < map->tilecount; gid++) {
		tmx_tile *tile = Map_GetTileInfo(map, gid);
		if (tile == nullptr) {
			continue;
		}

		totalSlots += 3;
		wrenEnsureSlots(vm, totalSlots);

		int propSlot = s++;
		wrenSetSlotNewMap(vm, propSlot);
		wrenInsertInList(vm, 0, -1, propSlot);

		wrenSetSlotString(vm, s++, tile->type == nullptr ? "" : tile->type);
		wrenInsertInMap(vm, propSlot, 1, s - 1);

		parseProperties(vm, propSlot, &totalSlots, &s, tile->properties);
	}
}

void wren_map_gettile(WrenVM *vm) {
	int layer = (int)wrenGetSlotDouble(vm, 1);
	unsigned int x = (unsigned int)wrenGetSlotDouble(vm, 2);
	unsigned int y = (unsigned int)wrenGetSlotDouble(vm, 3);
	
	int gid = Map_GetTile(map, layer, x, y);

	wrenSetSlotDouble(vm, 0, gid);
}

void wren_map_getlayernames(WrenVM *vm) {
	int i = 0;

	wrenSetSlotNewList(vm, 0);
	tmx_layer *layer = Map_GetLayer(map, i);
	while (layer != nullptr) {
		wrenEnsureSlots(vm, i + 2);
		wrenSetSlotString(vm, i + 1, layer->name);
		wrenInsertInList(vm, 0, -1, i + 1);
		i++;
		layer = layer->next;
	}
}
#pragma endregion

#pragma region Wren config callbacks
static bool clearNextError = true;
static void wren_error(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message) {
	if (type == WREN_ERROR_STACK_COMPLETE) {
		clearNextError = true;
		return;
	}

	if (clearNextError) {
		trap->Cvar_Set("com_lastErrorStack", "");
		clearNextError = false;
	}

	cvar_t *stack = trap->Cvar_Get("com_lastErrorStack", "", 0);
	if (line == -1) {
		trap->Cvar_Set("com_lastErrorStack", va("%s\n%s", stack->string, message));
		trap->Print("%s\n", message);
	}
	else {
		trap->Cvar_Set("com_lastErrorStack", va("%s\n(%s:%i) %s", stack->string, module, line, message));
		trap->Print("(%s:%i) %s\n", module, line, message);
	}
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
	{ "engine", "Trap", true, "printWin_(_,_,_)", wren_trap_dbgwin },
	{ "engine", "Trap", true, "error(_,_)", wren_trap_error },
	{ "engine", "Trap", true, "console(_)", wren_trap_console },
	{ "engine", "Trap", true, "sndPlay(_,_,_,_)", wren_trap_snd_play },
	{ "engine", "Trap", true, "sndStop(_)", wren_trap_snd_stop },
	{ "engine", "Trap", true, "keyActive(_)", wren_trap_in_keystate },
	{ "engine", "Trap", true, "keyPressed(_,_,_)", wren_trap_in_keypressed },
	{ "engine", "Trap", true, "mousePosition()", wren_trap_mouse_position },
	{ "engine", "Trap", true, "inspect(_)", wren_trap_inspect_instance },

	{ "engine", "Asset", true, "create(_,_,_)", wren_asset_create },
	{ "engine", "Asset", true, "find(_)", wren_asset_find },
	{ "engine", "Asset", true, "loadAll()", wren_asset_loadall },
	{ "engine", "Asset", true, "clearAll()", wren_asset_clearall },
	{ "engine", "Asset", true, "bmpfntSet(_,_,_,_,_,_)", wren_asset_bmpfnt_set },
	{ "engine", "Asset", true, "measureBmpText(_,_,_)", wren_asset_measurebmptext },
	{ "engine", "Asset", true, "createSprite(_,_,_,_,_)", wren_asset_create_sprite },

	{ "engine", "Draw", true, "setColor(_,_,_,_,_)", wren_dc_setcolor },
	{ "engine", "Draw", true, "resetTransform()", wren_dc_reset_transform },
	{ "engine", "Draw", true, "transform(_,_,_,_,_,_)", wren_dc_transform },
	{ "engine", "Draw", true, "rotate(_)", wren_dc_rotate },
	{ "engine", "Draw", true, "translate(_,_)", wren_dc_translate },
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

	{ "engine", "TileMap", true, "load(_)", wren_map_load },
	{ "engine", "TileMap", true, "free()", wren_map_free },
	{ "engine", "TileMap", true, "layerByName(_)", wren_map_getlayerbyname },
	{ "engine", "TileMap", true, "layerNames()", wren_map_getlayernames },
	{ "engine", "TileMap", true, "objectsInLayer(_)", wren_map_getobjectsinlayer },
	{ "engine", "TileMap", true, "getMapProperties()", wren_map_getmapproperties },
	{ "engine", "TileMap", true, "getLayerProperties(_)", wren_map_getlayerproperties },
	{ "engine", "TileMap", true, "getTileProperties()", wren_map_gettileproperties },
	{ "engine", "TileMap", true, "getTile(_,_,_)", wren_map_gettile },
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
#pragma endregion

#pragma region Public functions
WrenVM *Wren_Init(const char *mainScriptName, const char *constructorStr) {
	WrenConfiguration config;
	wrenInitConfiguration(&config);
	config.errorFn = wren_error;
	config.bindForeignMethodFn = wren_bindForeignMethodFn;
	config.loadModuleFn = wren_loadModuleFn;

	WrenVM *vm = wrenNewVM(&config);

	// load passed in script name as our main function
	char *mainStr;
	int mainSz = trap->FS_ReadFile(mainScriptName, (void**)&mainStr);
	if (mainSz <= 0) {
		trap->Error(ERR_DROP, "wren error: couldn't load %s", mainScriptName);
		return nullptr;
	}

	if (wrenInterpret(vm, mainStr) != WREN_RESULT_SUCCESS) {
		trap->Error(ERR_DROP, "wren error: can't compile %s", mainScriptName);
		return nullptr;
	}
	free(mainStr);

	// make sure we can find a new Game class
	wrenEnsureSlots(vm, 1);
	wrenGetVariable(vm, "main", "Main", 0);
	WrenHandle *gameClass = wrenGetSlotHandle(vm, 0);

	if (gameClass == nullptr) {
		trap->Error(ERR_DROP, "wren error: couldn't find Game class");
		return nullptr;
	}

	// make a new instance of the Game class and grab handles to update/draw
	WrenHandle *newHnd = wrenMakeCallHandle(vm, "init(_)");

	wrenHandles_t *hnd = new wrenHandles_t();
	hnd->instanceHnd = gameClass;
	hnd->updateHnd = wrenMakeCallHandle(vm, "update(_)");
	hnd->drawHnd = wrenMakeCallHandle(vm, "draw(_,_)");
	hnd->shutdownHnd = wrenMakeCallHandle(vm, "shutdown()");
	hnd->consoleHnd = wrenMakeCallHandle(vm, "console(_)");

	if (hnd->updateHnd == nullptr) {
		trap->Error(ERR_DROP, "wren error: couldn't find static update(_) on Main");
		return nullptr;
	}

	if (hnd->drawHnd == nullptr) {
		trap->Error(ERR_DROP, "wren error: couldn't find static draw(_,_) on Main");
		return nullptr;
	}

	// instantiate a new Game
	wrenEnsureSlots(vm, 8192); // FIXME: crash when expanding stack from inside constructor
	wrenSetSlotHandle(vm, 0, gameClass);
	if (constructorStr == nullptr) {
		wrenSetSlotNull(vm, 1);
	}
	else {
		wrenSetSlotString(vm, 1, constructorStr);
	}
	wrenCall(vm, newHnd);
	wrenReleaseHandle(vm, newHnd);
	//wrenReleaseHandle(vm, gameClass);

	if (wrenGetSlotCount(vm) == 0) {
		trap->Error(ERR_DROP, "wren error: couldn't instantiate new Game class");
		return nullptr;
	}

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

void Wren_Console(WrenVM *vm, const char *str) {
	wrenHandles_t* hnd = (wrenHandles_t*)wrenGetUserData(vm);
	wrenEnsureSlots(vm, 2);
	wrenSetSlotHandle(vm, 0, hnd->instanceHnd);
	wrenSetSlotString(vm, 1, str);
	wrenCall(vm, hnd->consoleHnd);
}

void Wren_Eval(WrenVM *vm, const char *code) {
	wrenInterpret(vm, code);
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
	wrenReleaseHandle(vm, hnd->consoleHnd);

	delete hnd;

	wrenFreeVM(vm);
}
#pragma endregion