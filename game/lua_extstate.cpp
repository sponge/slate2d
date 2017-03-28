#include "public.h"
#include "lua_extstate.h"

LuaExt lua;

static int physfs_searcher(lua_State* st) {
	const char* file = lua_tostring(st, 1);

	const char *buffer;
	int sz;

	sz = trap->FS_ReadFile(va("scripts/%s", file), (void **)&buffer);
	if (sz == -1) {
		sz = trap->FS_ReadFile(va("scripts/%s.lua", file), (void **)&buffer);
	}

	if (sz == -1) {
		return 0;
	}

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

static int console_print(lua_State* L) {
	int nargs = lua_gettop(L);

	for (int i = 1; i <= nargs; i++) {
		int t = lua_type(L, i);
		switch (t) {
		case LUA_TSTRING:
			trap->Print(va("string: %s\n", lua_tostring(L, i)));
			break;
		case LUA_TBOOLEAN:
			trap->Print(va("%s: %s\n", luaL_typename(L, i), lua_toboolean(L, i) ? "true" : "false"));
			break;
		case LUA_TNUMBER:
			trap->Print(va("%s: %0.5f\n", luaL_typename(L, i), lua_tonumber(L, i)));
			break;
		default:
			trap->Print(va("%s: %p\n", luaL_typename(L, i), lua_topointer(L, i)));
			break;
		}
	}

	return 0;
}

LuaExt::LuaExt() {
	auto st = this->lua_state();
	lua.open_libraries();
	
	static const struct luaL_Reg printlib[] = {
		{ "print", console_print },
		{ "fs_require", physfs_searcher},
		{ NULL, NULL }
	};

	lua_getglobal(st, "_G");
	luaL_setfuncs(st, printlib, 0);
	lua_pop(st, 1);

	lua.script("table.insert(package.searchers, 1, fs_require)");
}

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