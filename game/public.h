#include <nanovg.h>

// FIXME: getting functions not just types
#include "../src/image.h"
#include "../src/scene.h"
#include "../src/console/console.h" // FIXME: getting function defs that it shouldn't, just need cvar_t really

typedef struct gameImportFuncs_s {
	void (*SendConsoleCommand)(const char * text);
	void (*Print)(const char * text);
	cvar_t* (*Cvar_FindVar)(const char * name);
	void (*Cvar_Set)(const char *var_name, const char *value);
	void (*Cmd_AddCommand)(const char *cmd_name, xcommand_t function);
	int(*Cmd_Argc)(void);
	const char * (*Cmd_Argv)(int arg);
	const char * (*Cmd_ArgsFrom)(int arg);
	const char * (*Cmd_Cmd)();
	int  (*FS_ReadFile)(const char *path, void **buffer);
	bool (*FS_Exists)(const char *file);
	char** (*FS_List)(const char *path);
	void (*FS_FreeList)(void * listVar);
	Img* (*Img_Create)(const char * name, const char * path);
	void (*Img_Load)(NVGcontext * nvg, Img & img);
	void (*Img_LoadAll)(NVGcontext * nvg);
	Img* (*Img_Find)(const char * name);
	bool (*Img_Free)(const char * name);
	void (*Scene_Switch)(Scene *newScene);
	Scene* (*Scene_Get)(int i);
	void (*Scene_Replace)(int i, Scene *newScene);
	Scene* (*Scene_Current)();
} gameImportFuncs_t;

typedef struct gameExportFuncs_s {
	void(*Init)(void *clientInfo, void *imGuiContext);
} gameExportFuncs_t;

extern gameImportFuncs_t *trap;