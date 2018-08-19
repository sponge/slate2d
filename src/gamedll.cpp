#include <SDL/SDL.h>
#include <tmx.h>

#include "gamedll.h"
#include "../game/public.h"
#include "console/console.h"
#include "main.h"
#include "files.h"
#include "scene.h"
#include "input.h"
#include "rendercommands.h"
#include "bitmapfont.h"
#include "audio.h"
#include "assetloader.h"
#include "filetracker.h"

extern SceneManager *sm;
extern tmx_map *map;
extern ClientInfo inf;

void trap_SendConsoleCommand(const char *text) {
	Cbuf_ExecuteText(EXEC_NOW, text);
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

void * tmx_img_load(const char *path) {
	const char *fullpath = va("maps/%s", path);
	AssetHandle handle = Asset_Create(ASSET_IMAGE, fullpath, fullpath);
	return (void*)Asset_Get(ASSET_IMAGE, handle);
}

void tmx_img_free(void *address) {
	
}

void *tmx_fs(const char *filename, int *outSz) {
	void *xml;

	*outSz = FS_ReadFile(va("maps/%s",filename), &xml);

	if (*outSz < 0) {
		Com_Error(ERR_DROP, "Couldn't load file while parsing map %s", filename);
		return nullptr;
	}

	return xml;
}

tmx_map * trap_Map_Load(const char *filename) {
	tmx_img_load_func = &tmx_img_load;
	tmx_img_free_func = &tmx_img_free;
	tmx_file_read_func = &tmx_fs;

	const char *xml;
	int outSz = FS_ReadFile(filename, (void **) &xml);
	if (outSz < 0) {
		Com_Error(ERR_DROP, "Couldn't read map %s", filename);
		return nullptr;
	}

	map = tmx_load_buffer(xml, outSz);

	if (map == nullptr) {
		Com_Error(ERR_DROP, "Failed to load tmx");
		return nullptr;
	}

	if (map->orient != O_ORT) {
		Com_Error(ERR_DROP, "Non orthagonal tiles not supported");
		return nullptr;
	}

	return map;
}

void trap_Map_Free(tmx_map *localMap) {
	tmx_map_free(localMap);
	map = nullptr;
}

int R_RegisterShader(const char *name, const char *vshader, const char *fshader) {
	return 0;
	//return nvglCreateShaderGL3(inf.nvg, name, vshader, fshader);
}

static gameImportFuncs_t GAMEtraps = {
	trap_SendConsoleCommand,
	Com_Printf,
	Com_Error,
	Cvar_Get,
	Cvar_FindVar,
	Cvar_Set,
	Cmd_AddCommand,
	Cmd_Argc,
	Cmd_Argv,
	Cmd_ArgsFrom,
	Cmd_Cmd,
	FS_ReadFile,
	FS_Exists,
	FS_List,
	FS_FreeList,
	trap_Scene_Switch,
	trap_Scene_Get,
	trap_Scene_Replace,
	trap_Scene_Current,
	trap_Map_Load,
	trap_Map_Free,
	IN_KeyDown,
	IN_KeyUp,
	IN_KeyState,
	IN_KeyPressed,
	IN_MousePosition,
	SubmitRenderCommands,
	R_RegisterShader,
	Asset_Create,
	Asset_Find,
	Asset_LoadAll,
	Asset_ClearAll,
	BMPFNT_Set,
	BMPFNT_TextWidth,
	Sprite_Set,
	Get_Img,
	Snd_Play,
	Snd_Stop,
	Snd_PauseResume,
	FileWatcher_TrackFile
};

#ifdef __EMSCRIPTEN__
extern "C" void dllEntry(void ** exports, void * imports, int * version);
void Sys_LoadDll(const char * module, void ** exports, int * version) {
	dllEntry(exports, &GAMEtraps, version);
}
#else
void Sys_LoadDll(const char * module, void ** exports, int * version) {
	void *gameDLL;

	gameDLL = SDL_LoadObject(va("%s/%s/%s", fs_basepath->string, fs_game->string, module));

	if (gameDLL == nullptr) {
		gameDLL = SDL_LoadObject(va("%s/%s/%s", fs_basepath->string, fs_basegame->string, module));
	}

	if (gameDLL == nullptr) {
		gameDLL = SDL_LoadObject(va("%s/%s", fs_basepath->string, module));
	}

	if (gameDLL == nullptr) {
		gameDLL = SDL_LoadObject(module);
	}

	void(*gameDllEntry)(void ** exports, const void * imports, int * version) = (void(*)(void **, const void *, int *)) SDL_LoadFunction(gameDLL, "dllEntry");

	if (gameDllEntry == nullptr) {
		Com_Error(ERR_FATAL, "Couldn't find dllEntry in %s", module);
	}

	gameDllEntry(exports, &GAMEtraps, version);

}
#endif
