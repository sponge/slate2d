#include <imgui.h>
#include "public.h"
#include "draw.h"
#include <stdio.h>
#include "scene_game.h"

gameImportFuncs_t *trap;
kbutton_t in_1_left, in_1_right, in_1_up, in_1_run, in_1_down, in_1_jump, in_1_attack, in_1_menu;

void Com_DefaultExtension(char *path, int maxSize, const char *extension);

// map (name) - load a map and switch to the game scene
void Cmd_Map_f(void) {
	auto mapname = trap->Cmd_Argv(1);
	char filename[256];

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

	auto newScene = new GameScene(filename);
	trap->Scene_Replace(0, newScene);
}

static void Init(void *clientInfo, void *imGuiContext) {
	ClientInfo *inf = (ClientInfo*)clientInfo;
	inf->gameWidth = 320;
	inf->gameHeight = 180;

	trap->Cmd_AddCommand("map", Cmd_Map_f);

	trap->Cmd_AddCommand("+p1up",     []() { trap->IN_KeyDown(&in_1_up); });
	trap->Cmd_AddCommand("-p1up",     []() { trap->IN_KeyUp(&in_1_up); });
	trap->Cmd_AddCommand("+p1down",   []() { trap->IN_KeyDown(&in_1_down); });
	trap->Cmd_AddCommand("-p1down",   []() { trap->IN_KeyUp(&in_1_down); });
	trap->Cmd_AddCommand("+p1left",   []() { trap->IN_KeyDown(&in_1_left); });
	trap->Cmd_AddCommand("-p1left",   []() { trap->IN_KeyUp(&in_1_left); });
	trap->Cmd_AddCommand("+p1right",  []() { trap->IN_KeyDown(&in_1_right); });
	trap->Cmd_AddCommand("-p1right",  []() { trap->IN_KeyUp(&in_1_right); });
	trap->Cmd_AddCommand("+p1run",    []() { trap->IN_KeyDown(&in_1_run); });
	trap->Cmd_AddCommand("-p1run",    []() { trap->IN_KeyUp(&in_1_run); });
	trap->Cmd_AddCommand("+p1jump",   []() { trap->IN_KeyDown(&in_1_jump); });
	trap->Cmd_AddCommand("-p1jump",   []() { trap->IN_KeyUp(&in_1_jump); });
	trap->Cmd_AddCommand("+p1attack", []() { trap->IN_KeyDown(&in_1_attack); });
	trap->Cmd_AddCommand("-p1attack", []() { trap->IN_KeyUp(&in_1_attack); });
	trap->Cmd_AddCommand("+p1menu",   []() { trap->IN_KeyDown(&in_1_menu); });
	trap->Cmd_AddCommand("-p1menu",   []() { trap->IN_KeyUp(&in_1_menu); });

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	trap->SendConsoleCommand("map dognew");
}

static void Console(const char *line) {

}

// technically the scene manager will handle every frame for gameplay scenes,
// but anything that needs an event loop type pump can go here
static void Frame(float dt) {

}

static gameExportFuncs_t GAMEfuncs = {
	Init,
	Console,
	Frame
};

extern "C" 
#ifdef _WIN32
__declspec(dllexport)
#endif
void dllEntry(void ** exports, void * imports, int * version) {
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