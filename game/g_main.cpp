#include <imgui.h>
#include "public.h"
#include "draw.h"
#include <stdio.h>
#include "scene_wren.h"
#include "game.h"

ClientInfo *clientInf;
gameImportFuncs_t *trap;
kbutton_t buttons[MAX_KEYS];
const char *buttoncmds[MAX_KEYS] = { "p1up", "p1down", "p1left", "p1right", "p1a", "p1b", "p1x", "p1y", "p1l", "p1r", "p1start", "p1select" };
Scene *scene;

#ifdef __EMSCRIPTEN__
extern
#endif
void Com_DefaultExtension(char *path, int maxSize, const char *extension);

void Cmd_Scene_f(void) {
	const char *mainScriptName = trap->Cmd_Argv(1);
	const char *sceneParams = trap->Cmd_ArgsFrom(2);
	if (sceneParams[0] == '\0') {
		sceneParams = nullptr;
	}

	auto newScene = new WrenScene(mainScriptName, sceneParams);
	if (scene) { delete scene; }
	scene = newScene;
	scene->Startup(clientInf);
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

	auto newScene = new WrenScene("scripts/main.wren", filename);
	if (scene) { delete scene; }
	scene = newScene;
	scene->Startup(clientInf);
}

static void Init(void *clientInfo, void *imGuiContext) {
	clientInf = (ClientInfo*) clientInfo;

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	auto newScene = new WrenScene("scripts/main.wren", nullptr);
	if (scene) { delete scene; }
	scene = newScene;
	scene->Startup(clientInf);
}

static bool Console(const char *line) {
	const char *cmd = trap->Cmd_Argv(0);
	// if it's a + or - command, look to see if its a known key
	// and signal to the engine that it's been pressed.
	// we do this here so the game dll can customize the buttons used
	if (line[0] == '+' || line[0] == '-') {
		for (int i = 0; i < MAX_KEYS; i++) {
			if (strcasecmp(cmd+1, buttoncmds[i]) == 0) {
				if (cmd[0] == '+') {
					trap->IN_KeyDown(&buttons[i]);
				}
				else {
					trap->IN_KeyUp(&buttons[i]);
				}

				return true;
			}
		}

		return false;
	}

	// search for known commands (this could be an array but we don't have
	// enough to make it worth it.)
	if (strcasecmp(cmd, "map") == 0) { Cmd_Map_f(); return true; }
	if (strcasecmp(cmd, "scene") == 0) { Cmd_Scene_f(); return true; }

	// if there's a scene loaded and it's not a /, pass it into the scene
	// which is usually handled by eval'ing wren code
	if (scene != nullptr && line[0] != '/') {
		scene->Console(line);
		return true;
	}

	return false;	
}

static void Frame(double dt) {
	if (scene == nullptr) {
		return;
	}

	if (dt != 0) {
		scene->Update(dt);
	}

	scene->Render();
}

static void Error(int level, const char *msg) {
	NOTUSED(msg);
	NOTUSED(level);
	delete scene;
	scene = nullptr;
}

static gameExportFuncs_t gameExports = {
	Init,
	Console,
	Frame,
	Error
};

extern "C" 
#ifdef _WIN32
__declspec(dllexport)
#endif
void dllEntry(void ** exports, void * imports) {
	*exports = &gameExports;
	trap = (gameImportFuncs_t *)imports;
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
