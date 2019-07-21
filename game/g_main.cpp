#include <imgui.h>
#include "../src/shared.h"
#include <stdio.h>
#include "game.h"
#include "external/sds.h"
#include "wrenapi.h"
#include <setjmp.h>

const ClientInfo *clientInf;
WrenVM *vm;
static bool loop = true;
jmp_buf env;

void Com_DefaultExtension(char *path, int maxSize, const char *extension);

void Cmd_Scene_f(void) {
	const char *mainScriptName = SLT_Con_GetArg(1);
	const char *sceneParams = SLT_Con_GetArgs(2);
	if (sceneParams[0] == '\0') {
		sceneParams = nullptr;
	}

	if (vm != nullptr) {
		Wren_Scene_Shutdown(vm);
		Wren_FreeVM(vm);
	}

	vm = Wren_Init(mainScriptName, sceneParams);
	if (vm != nullptr) {
		SLT_Con_SetVar("engine.errorMessage", "");
	}
}

// map (name) - load a map and switch to the game scene
void Cmd_Map_f(void) {
	auto mapname = SLT_Con_GetArg(1);
	char filename[256];

	SLT_Con_SetVar("engine.lastErrorStack", "");
	SLT_Con_SetVar("engine.errorMessage", "");

	if (SLT_Con_GetArgCount() != 2) {
		SLT_Print("map <mapname> : load a map\n");
		return;
	}

	snprintf(filename, sizeof(filename), "maps/%s", mapname);
	Com_DefaultExtension(filename, sizeof(filename), ".tmx");

	if (!SLT_FS_Exists(filename)) {
		SLT_Print("Map does not exist: %s\n", filename);
		return;
	}

	if (vm != nullptr) {
		Wren_Scene_Shutdown(vm);
		Wren_FreeVM(vm);
	}

	vm = Wren_Init("scripts/main.wren", filename);
	if (vm != nullptr) {
		SLT_Con_SetVar("engine.errorMessage", "");
	}
}

void Cmd_Eval_f(void) {
	if (vm == nullptr) {
		return;
	}

	const char* line = SLT_Con_GetArg(1);
	Wren_Console(vm, line);
}

static void Error(int level, const char *msg) {
	NOTUSED(msg);
	NOTUSED(level);
	
	if (vm) {
		Wren_FreeVM(vm);
		vm = nullptr;
	}

	SLT_Asset_ClearAll();
	if (env) {
		longjmp(env, 1);
	}
}

const char* __cdecl gtempstr(const char* format, ...) {
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char* buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

	return buf;
}

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

void main_loop() {
	bool ranUpdate = false;
	double dt = SLT_StartFrame();
	if (dt < 0) {
		loop = false;
		SLT_EndFrame();
		return;
	}
	else {
		if (vm == nullptr) {
			SLT_UpdateLastFrameTime();
			DC_Clear(0, 0, 0, 255);
			DC_Submit();
			SLT_EndFrame();
			return;
		}

		if (dt > 0) {
			ranUpdate = Wren_Update(vm, dt);
		}
	}

	Wren_Draw(vm, clientInf->width, clientInf->height);
	SLT_EndFrame();

	SLT_UpdateLastFrameTime();
}

int main(int argc, char* argv[]) {
	SLT_Init(argc, argv);

	SLT_Con_SetErrorHandler(Error);
	SLT_Con_AddCommand("map", Cmd_Map_f);
	SLT_Con_AddCommand("scene", Cmd_Scene_f);
	SLT_Con_AddCommand("eval", Cmd_Eval_f);

	clientInf = (ClientInfo*)SLT_GetClientInfo();
	ImGui::SetCurrentContext((ImGuiContext*)SLT_GetImguiContext());

	if (vm != nullptr) {
		Wren_Scene_Shutdown(vm);
		Wren_FreeVM(vm);
	}

	vm = Wren_Init("scripts/main.wren", nullptr);
	if (vm != nullptr) {
		SLT_Con_SetVar("engine.errorMessage", "");
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	setjmp(env);
	while (loop) {
		main_loop();
	}
#endif

	SLT_Shutdown();
}