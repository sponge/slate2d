#pragma once
#include "selene/selene.h"

class LuaExt : public sel::State {
public:
	LuaExt();
	bool LoadGameFile(const std::string &file);
};

extern LuaExt lua;