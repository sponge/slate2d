#include <imgui.h>

#include "public.h"
#include "scene_menu.h"
#include "scene_testbounce.h"

#include "lua_extstate.h"

gameImportFuncs_t *trap;

LuaExt lua;

void Cmd_Lua_f(void) {
	const char *line = trap->Cmd_Cmd();
	line += 4;
	auto res = lua.do_string(line);
	if (!res.valid()) {
		sol::error err = res;
		trap->Print(err.what());
	}
}

void Cmd_Scene_f(void) {
	auto num = atoi(trap->Cmd_Argv(1));

	if (num < 0 || num > 3) {
		trap->Print("invalid scene, specify a number 0-2\n");
		return;
	}
	Scene* newScene;

	switch (num) {
	case 0: default: newScene = new MenuScene(); break;
	case 1: newScene = new TestBounceScene(); break;
	//case 2: newScene = new GLScene(); break;
	}

	trap->Scene_Replace(0, newScene);
}

// FIXME: abstract out rendering so i can just pass a list of drawing commands instead of nvg context stuff
static void Init(void *clientInfo, void *imGuiContext) {
	trap->Cmd_AddCommand("scene", Cmd_Scene_f);
	trap->Cmd_AddCommand("lua", Cmd_Lua_f);

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	lua.LoadGameFile("scripts/autoexec.lua");

	trap->Scene_Switch(new MenuScene());
}

static gameExportFuncs_t GAMEfuncs = {
	Init,
};

// FIXME: windows only :(
extern "C" __declspec(dllexport) void dllEntry(void ** exports, void * imports, int * version) {
	*exports = &GAMEfuncs;
	trap = (gameImportFuncs_t *)imports;
	*version = 1;
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
const char	* __cdecl va(const char *format, ...) {
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char	*buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

	return buf;
}