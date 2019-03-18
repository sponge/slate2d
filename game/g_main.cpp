#include <imgui.h>
#include "../src/shared.h"
#include "draw.h"
#include <stdio.h>
#include "game.h"
#include "external/sds.h"
#include "wrenapi.h"

ClientInfo *clientInf;
gameImportFuncs_t *trap;
WrenVM *vm;

#ifdef __EMSCRIPTEN__
extern
#endif
void Com_DefaultExtension(char *path, int maxSize, const char *extension);

void Cmd_Scene_f(void) {
	const char *mainScriptName = trap->Con_GetArg(1);
	const char *sceneParams = trap->Con_GetArgs(2);
	if (sceneParams[0] == '\0') {
		sceneParams = nullptr;
	}

	if (vm != nullptr) {
		Wren_Scene_Shutdown(vm);
		Wren_FreeVM(vm);
	}

	vm = Wren_Init(mainScriptName, sceneParams);
	if (vm != nullptr) {
		trap->Con_SetVar("engine.errorMessage", "");
	}
}

// map (name) - load a map and switch to the game scene
void Cmd_Map_f(void) {
	auto mapname = trap->Con_GetArg(1);
	char filename[256];

	trap->Con_SetVar("engine.lastErrorStack", "");
	trap->Con_SetVar("engine.errorMessage", "");

	if (trap->Con_GetArgCount() != 2) {
		trap->Print("map <mapname> : load a map\n");
		return;
	}

	snprintf(filename, sizeof(filename), "maps/%s", mapname);
	Com_DefaultExtension(filename, sizeof(filename), ".tmx");

	if (!trap->FS_Exists(filename)) {
		trap->Print("Map does not exist: %s\n", filename);
		return;
	}

	if (vm != nullptr) {
		Wren_Scene_Shutdown(vm);
		Wren_FreeVM(vm);
	}

	vm = Wren_Init("scripts/main.wren", filename);
	if (vm != nullptr) {
		trap->Con_SetVar("engine.errorMessage", "");
	}
}

static void Init(void *clientInfo, void *imGuiContext) {
	clientInf = (ClientInfo*) clientInfo;

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	if (vm != nullptr) {
		Wren_Scene_Shutdown(vm);
		Wren_FreeVM(vm);
	}

	vm = Wren_Init("scripts/main.wren", nullptr);
	if (vm != nullptr) {
		trap->Con_SetVar("engine.errorMessage", "");
	}
}

static bool Console() {
	const char *cmd = trap->Con_GetArg(0);
	const char *line = trap->Con_GetArgs(1);

	// search for known commands (this could be an array but we don't have
	// enough to make it worth it.)
	if (strcasecmp(cmd, "map") == 0) { Cmd_Map_f(); return true; }
	if (strcasecmp(cmd, "scene") == 0) { Cmd_Scene_f(); return true; }
	if (strcasecmp(cmd, "eval") == 0) { Wren_Console(vm, line); return true; }

	return false;	
}

static bool Frame(double dt) {
	if (vm == nullptr) {
		return true;
	}

	bool ranUpdate = false;
	if (dt != 0) {
		ranUpdate = Wren_Update(vm, dt);
	}

	Wren_Draw(vm, clientInf->width, clientInf->height);

	return ranUpdate;
}

static void Error(int level, const char *msg) {
	NOTUSED(msg);
	NOTUSED(level);
	
	if (vm) {
		Wren_FreeVM(vm);
		vm = nullptr;
	}
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

#ifndef __EMSCRIPTEN__
const char * __cdecl tempstr(const char *format, ...) {
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
