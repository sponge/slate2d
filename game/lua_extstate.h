#pragma once
#define SOL_NO_EXCEPTIONS
#include "sol.h"

class LuaExt : public sol::state {
public:
	LuaExt();
	bool LoadGameFile(const std::string &file);
};

extern LuaExt lua;