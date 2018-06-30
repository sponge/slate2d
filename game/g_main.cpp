#include <imgui.h>
#include "public.h"
#include "draw.h"
#include <stdio.h>
#include "scene_game.h"
#include "scene_wren.h"
#include "game.h"

gameImportFuncs_t *trap;
kbutton_t buttons[12];
#ifdef __EMSCRIPTEN__
extern ClientInfo *inf;
#else
ClientInfo *inf;
#endif
WrenScene *wrenScene;

#ifdef __EMSCRIPTEN__
extern
#endif
void Com_DefaultExtension(char *path, int maxSize, const char *extension);

void Cmd_Scene_f(void) {
	const char *mainScriptName = trap->Cmd_Argv(1);
	const char *sceneParams = trap->Cmd_ArgsFrom(2);
	if (strlen(sceneParams) == 0) {
		sceneParams = nullptr;
	}

	auto newScene = new WrenScene(mainScriptName, sceneParams);
	trap->Scene_Replace(0, newScene);
}

// map (name) - load a map and switch to the game scene
void Cmd_Map_f(void) {
	auto mapname = trap->Cmd_Argv(1);
	char filename[256];

	trap->Cvar_Set("com_lastErrorStack", "");
	trap->Cvar_Set("com_errorMessage", "");

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

	wrenScene = new WrenScene("scripts/main.wren", filename);
	trap->Scene_Replace(0, wrenScene);
}

static void Init(void *clientInfo, void *imGuiContext) {
	trap->Cmd_AddCommand("map", Cmd_Map_f);
	trap->Cmd_AddCommand("scene", Cmd_Scene_f);

	trap->Cmd_AddCommand("+p1up",		[]() { trap->IN_KeyDown(&buttons[0]); });
	trap->Cmd_AddCommand("-p1up",		[]() { trap->IN_KeyUp(&buttons[0]); });
	trap->Cmd_AddCommand("+p1down",		[]() { trap->IN_KeyDown(&buttons[1]); });
	trap->Cmd_AddCommand("-p1down",		[]() { trap->IN_KeyUp(&buttons[1]); });
	trap->Cmd_AddCommand("+p1left",		[]() { trap->IN_KeyDown(&buttons[2]); });
	trap->Cmd_AddCommand("-p1left",		[]() { trap->IN_KeyUp(&buttons[2]); });
	trap->Cmd_AddCommand("+p1right",	[]() { trap->IN_KeyDown(&buttons[3]); });
	trap->Cmd_AddCommand("-p1right",	[]() { trap->IN_KeyUp(&buttons[3]); });
	trap->Cmd_AddCommand("+p1a",		[]() { trap->IN_KeyDown(&buttons[4]); });
	trap->Cmd_AddCommand("-p1a",		[]() { trap->IN_KeyUp(&buttons[4]); });
	trap->Cmd_AddCommand("+p1b",		[]() { trap->IN_KeyDown(&buttons[5]); });
	trap->Cmd_AddCommand("-p1b",		[]() { trap->IN_KeyUp(&buttons[5]); });
	trap->Cmd_AddCommand("+p1x",		[]() { trap->IN_KeyDown(&buttons[6]); });
	trap->Cmd_AddCommand("-p1x",		[]() { trap->IN_KeyUp(&buttons[6]); });
	trap->Cmd_AddCommand("+p1y",		[]() { trap->IN_KeyDown(&buttons[7]); });
	trap->Cmd_AddCommand("-p1y",		[]() { trap->IN_KeyUp(&buttons[7]); });
	trap->Cmd_AddCommand("+p1l",		[]() { trap->IN_KeyDown(&buttons[8]); });
	trap->Cmd_AddCommand("-p1l",		[]() { trap->IN_KeyUp(&buttons[8]); });
	trap->Cmd_AddCommand("+p1r",		[]() { trap->IN_KeyDown(&buttons[9]); });
	trap->Cmd_AddCommand("-p1r",		[]() { trap->IN_KeyUp(&buttons[9]); });
	trap->Cmd_AddCommand("+p1start",	[]() { trap->IN_KeyDown(&buttons[10]); });
	trap->Cmd_AddCommand("-p1start",	[]() { trap->IN_KeyUp(&buttons[10]); });
	trap->Cmd_AddCommand("+p1select",	[]() { trap->IN_KeyDown(&buttons[11]); });
	trap->Cmd_AddCommand("-p1select",	[]() { trap->IN_KeyUp(&buttons[11]); });

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	wrenScene = new WrenScene("scripts/main.wren", nullptr);
	trap->Scene_Replace(0, wrenScene);
}

static void Console(const char *line) {
	if (wrenScene != nullptr) {
		wrenScene->Console(line);
	}
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
#ifndef __EMSCRIPTEN__

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
#endif
