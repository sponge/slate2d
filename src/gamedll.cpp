#include <SDL/SDL.h>

#include "gamedll.h"
#include "../game/public.h"
#include "console/console.h"
#include "main.h"
#include "image.h"
#include "files.h"
#include "scene.h"

extern SceneManager *sm;

void trap_SendConsoleCommand(const char *text) {
	Cbuf_ExecuteText(EXEC_NOW, text);
}

void trap_Print(const char *text) {
	Com_Printf("%s", text);
}

cvar_t * trap_Cvar_FindVar(const char *name) {
	return Cvar_FindVar(name);
}

void trap_Scene_Switch(Scene *newScene) {
	sm->Switch(newScene);
}

Scene * trap_Scene_Get(int i) {
	return sm->Get(i);
}

void trap_Scene_Replace(int i, Scene *newScene) {
	sm->Replace(i, newScene);
}

Scene * trap_Scene_Current() {
	return sm->Current();
}

static gameImportFuncs_t GAMEtraps = {
	trap_SendConsoleCommand,
	trap_Print,
	trap_Cvar_FindVar,
	Cvar_Set,
	Cmd_AddCommand,
	Cmd_Argc,
	Cmd_Argv,
	FS_ReadFile,
	FS_Exists,
	FS_List,
	FS_FreeList,
	Img_Create,
	Img_Load,
	Img_LoadAll,
	Img_Find,
	Img_Free,
	trap_Scene_Switch,
	trap_Scene_Get,
	trap_Scene_Replace,
	trap_Scene_Current
};

void Sys_LoadDll(const char * module, void ** exports, int * version) {
	auto gameDLL = SDL_LoadObject(module);

	void(*gameDllEntry)(void ** exports, const void * imports, int * version) = (void(*)(void **, const void *, int *)) SDL_LoadFunction(gameDLL, "dllEntry");

	if (gameDllEntry == nullptr) {
		Com_Error(ERR_FATAL, "Couldn't find dllEntry in %s", module);
	}

	gameDllEntry(exports, &GAMEtraps, version);

}
