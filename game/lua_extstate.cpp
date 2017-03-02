#include "public.h"
#include "lua_extstate.h"

static int l_my_print(lua_State* L) {
	int nargs = lua_gettop(L);

	for (int i = 1; i <= nargs; i++) {
		if (lua_isstring(L, i)) {
			trap->Print(lua_tostring(L, i));
		}
		else {
			/* Do something with non-strings if you like */
		}
	}

	return 0;
}

LuaExt::LuaExt() : sel::State(true) {
	HandleExceptionsWith(sel::ExceptionHandler::function([](int, std::string msg, std::exception_ptr) { trap->Print(msg.c_str()); }));

	static const struct luaL_Reg printlib[] = {
		{ "print", l_my_print },
		{ NULL, NULL }
	};

	luaL_openlibs(_l);
	lua_getglobal(_l, "_G");
	luaL_setfuncs(_l, printlib, 0);
	lua_pop(_l, 1);
}

bool LuaExt::LoadGameFile(const std::string &file) {
	const char *buffer;
	int sz = trap->FS_ReadFile(file.c_str(), (void **)&buffer);
	auto status = luaL_loadbuffer(this->_l, buffer, sz, file.c_str());

	if (status != LUA_OK) {
		if (status == LUA_ERRSYNTAX) {
			const char *msg = lua_tostring(_l, -1);
			_exception_handler->Handle(status, msg ? msg : file + ": syntax error");
		}
		else if (status == LUA_ERRFILE) {
			const char *msg = lua_tostring(_l, -1);
			_exception_handler->Handle(status, msg ? msg : file + ": file error");
		}
		return false;
	}

	status = lua_pcall(_l, 0, LUA_MULTRET, 0);
	if (status == LUA_OK) {
		return true;
	}

	const char *msg = lua_tostring(_l, -1);
	_exception_handler->Handle(status, msg ? msg : file + ": dofile failed");
	return false;
}