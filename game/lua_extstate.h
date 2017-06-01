#pragma once
#define SOL_NO_EXCEPTIONS
#include "sol.h"

// extend sol state to setup the environment to use engine/dll features
class LuaExt : public sol::state {
public:
	LuaExt();
	// convenience function to read and execute lua files from the vfs
	bool LoadGameFile(const std::string &file);
};

extern LuaExt lua;