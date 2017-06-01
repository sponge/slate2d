#include "public.h"
#include "lua_extstate.h"
#include "game.h"
#include "drawcommands.h"
extern "C" {
#include <luasocket.h>
#include <mime.h>
}

LuaExt lua;

// Quick macro for adding functions to the preloder.
#define PRELOAD(name, function) \
	lua_getglobal(st, "package"); \
	lua_getfield(st, -1, "preload"); \
	lua_pushcfunction(st, function); \
	lua_setfield(st, -2, name); \
	lua_pop(st, 2);	

// search paths for lua to search virtual fs from. must end in a nullptr!
const char * const searchPaths[] = {
	"scripts/%s",
	"scripts/%s.lua",
	"scripts/lib/%s",
	"scripts/lib/%s.lua",
	nullptr
};

// called by lua to resolve paths. use vfs here instead of filesystem.
static int physfs_searcher(lua_State* st) {
	const char* file = lua_tostring(st, 1);

	const char *buffer;
	int sz;

	const char *full = nullptr;
	for (int i = 0; searchPaths[i] != nullptr; i++) {
		const char *check = va(searchPaths[i], file);
		if (trap->FS_Exists(check)) {
			full = check;
			break;
		}
	}

	if (full == nullptr) {
		return 0;
	}

	sz = trap->FS_ReadFile(va(full, file), (void **)&buffer);
	auto status = luaL_loadbuffer(st, buffer, sz, file);

	if (status != LUA_OK) {
		if (status == LUA_ERRSYNTAX) {
			const char *msg = lua_tostring(st, -1);
			trap->Print(msg ? msg : va("%s: syntax error", file));
		}
		else if (status == LUA_ERRFILE) {
			const char *msg = lua_tostring(st, -1);
			trap->Print(msg ? msg : va("%s: file error", file));
		}
		return 0;
	}

	return 1;
}

// override print function to go through dll interface
static int console_print(lua_State* L) {
	int nargs = lua_gettop(L);

	for (int i = 1; i <= nargs; i++) {
		int t = lua_type(L, i);
		switch (t) {
		case LUA_TSTRING:
			trap->Print("%s\n", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			trap->Print("%s: %s\n", luaL_typename(L, i), lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			trap->Print("%s: %0.5f\n", luaL_typename(L, i), lua_tonumber(L, i));
			break;
		default:
			trap->Print("%s: %p\n", luaL_typename(L, i), lua_topointer(L, i));
			break;
		}
	}

	return 0;
}

LuaExt::LuaExt() {
	auto st = this->lua_state();

	// open all base libs
	lua.open_libraries();

	// open all third party libs
	PRELOAD("socket.core", luaopen_socket_core);
	PRELOAD("mime.core", luaopen_mime_core);

	static const struct luaL_Reg printlib[] = {
		{ "print", console_print },
		{ "fs_require", physfs_searcher},
		{ NULL, NULL }
	};

	lua_getglobal(st, "_G");
	luaL_setfuncs(st, printlib, 0);
	lua_pop(st, 1);

	// overwrite all searchers so all requires go through physfs
	lua.script("package.searchers = {package.searchers[1]}");
	lua.script("table.insert(package.searchers, 2, fs_require)");

	// expose draw commands to lua
	auto dct = lua.create_table_with();
	lua["dc"] = dct;
	dct["submit"] = DC_Submit;
	dct["clear"] = DC_Clear;
	dct["set_color"] = DC_SetColor;
	dct["set_transform"] = DC_SetTransform;
	dct["rect"] = DC_DrawRect;
	dct["text"] = DC_DrawText;
	dct["bmp_text"] = DC_DrawBmpText;
	dct["image"] = DC_DrawImage;

	// expose audio commands to lua. this should probably get better.
	lua["play_music"] = [](const char *file) { trap->SND_PlayMusic(file); };
	lua["play_sound"] = [](const char *file) { trap->SND_PlaySound(file); };
	lua["play_speech"] = [](const char *text) { trap->SND_PlaySpeech(text); };
}

// convenience function to read and execute lua files from the vfs
bool LuaExt::LoadGameFile(const std::string &file) {
	auto st = this->lua_state();

	const char *buffer;
	int sz = trap->FS_ReadFile(file.c_str(), (void **)&buffer);
	auto status = luaL_loadbuffer(st, buffer, sz, file.c_str());

	if (status != LUA_OK) {
		if (status == LUA_ERRSYNTAX) {
			const char *msg = lua_tostring(st, -1);
			trap->Print(msg ? msg : va("%s: syntax error", file.c_str()));
		}
		else if (status == LUA_ERRFILE) {
			const char *msg = lua_tostring(st, -1);
			trap->Print(msg ? msg : va("%s: file error", file.c_str()));
		}
		return false;
	}

	status = lua_pcall(st, 0, LUA_MULTRET, 0);
	if (status == LUA_OK) {
		return true;
	}

	const char *msg = lua_tostring(st, -1);
	trap->Print(msg ? msg : va("%s: dofile failed", file.c_str()));
	return false;
}