#include <imgui.h>

#define AABB_IMPLEMENTATION
#include "sweep.h"

#include "public.h"
#include "cvar_game.h"
#include "scene_menu.h"
#include "scene_gamemap.h"
#include "scene_testbounce.h"

#include "lua_extstate.h"

gameImportFuncs_t *trap;
kbutton_t in_1_left, in_1_right, in_1_up, in_1_down, in_1_jump, in_1_attack, in_1_menu;

void Cmd_Map_f(void) {
	auto mapname = trap->Cmd_Argv(1);
	char filename[MAX_QPATH];

	if (trap->Cmd_Argc() != 2) {
		trap->Print("map <mapname> : load a map\n");
		return;
	}

	snprintf(filename, sizeof(filename), "maps/%s", mapname);
	Com_DefaultExtension(filename, sizeof(filename), ".tmx");

	if (!trap->FS_Exists(filename)) {
		trap->Print("Map does not exist: %s\n", filename);
		return;
	}

	auto newScene = new GameMapScene(filename);
	
	trap->Scene_Replace(0, newScene);
}

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
	trap->Cmd_AddCommand("map", Cmd_Map_f);

	trap->Cmd_AddCommand("+p1up",     []() { trap->IN_KeyDown(&in_1_up); });
	trap->Cmd_AddCommand("-p1up",     []() { trap->IN_KeyUp(&in_1_up); });
	trap->Cmd_AddCommand("+p1down",   []() { trap->IN_KeyDown(&in_1_down); });
	trap->Cmd_AddCommand("-p1down",   []() { trap->IN_KeyUp(&in_1_down); });
	trap->Cmd_AddCommand("+p1left",   []() { trap->IN_KeyDown(&in_1_left); });
	trap->Cmd_AddCommand("-p1left",   []() { trap->IN_KeyUp(&in_1_left); });
	trap->Cmd_AddCommand("+p1right",  []() { trap->IN_KeyDown(&in_1_right); });
	trap->Cmd_AddCommand("-p1right",  []() { trap->IN_KeyUp(&in_1_right); });
	trap->Cmd_AddCommand("+p1jump",   []() { trap->IN_KeyDown(&in_1_jump); });
	trap->Cmd_AddCommand("-p1jump",   []() { trap->IN_KeyUp(&in_1_jump); });
	trap->Cmd_AddCommand("+p1attack", []() { trap->IN_KeyDown(&in_1_attack); });
	trap->Cmd_AddCommand("-p1attack", []() { trap->IN_KeyUp(&in_1_attack); });
	trap->Cmd_AddCommand("+p1menu",   []() { trap->IN_KeyDown(&in_1_menu); });
	trap->Cmd_AddCommand("-p1menu",   []() { trap->IN_KeyUp(&in_1_menu); });

	RegisterGameCvars();

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	lua.LoadGameFile("scripts/autoexec.lua");

	trap->Scene_Switch(new MenuScene());
}

static gameExportFuncs_t GAMEfuncs = {
	Init,
};

extern "C" 
#ifdef _WIN32
__declspec(dllexport)
#endif
void dllEntry(void ** exports, void * imports, int * version) {
#ifdef DEBUG
	testCollision();
#endif

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

/*
==================
Com_DefaultExtension
==================
*/
void Com_DefaultExtension(char *path, int maxSize, const char *extension) {
	char	oldPath[1024];
	char    *src;

	//
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path) {
		if (*src == '.') {
			return;                 // it has an extension
		}
		src--;
	}

	strncpy(oldPath, path, sizeof(oldPath));
	snprintf(path, maxSize, "%s%s", oldPath, extension);
}