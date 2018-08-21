#pragma once
#include <tmx.h>

#include "shared.h"

typedef struct gameImportFuncs_s {
	void (*SendConsoleCommand)(const char * text);
	void (*Print)(const char *fmt, ...);
	void(*Error)(int level, const char *error, ...);

	cvar_t* (*Cvar_Get)(const char *var_name, const char *var_value, int flags);
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

	void (*IN_KeyDown)(kbutton_t *b);
	void (*IN_KeyUp)(kbutton_t *b);
	float (*IN_KeyState)(kbutton_t *key);
	bool (*IN_KeyPressed)(kbutton_t *key, unsigned int delay, int repeat);
	MousePosition (*IN_MousePosition)();

	void(*SubmitRenderCommands)(renderCommandList_t *list);
	int(*R_RegisterShader)(const char *name, const char *vshader, const char *fshader);

	AssetHandle (*Asset_Create)(AssetType_t assetType, const char *name, const char *path, int flags);
	AssetHandle (*Asset_Find)(const char *name);
	void (*Asset_Load)(AssetHandle assetHandle);
	void (*Asset_LoadAll)();
	void (*Asset_ClearAll)();
	void (*Asset_BMPFNT_Set)(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);
	int (*Asset_BMPFNT_TextWidth)(AssetHandle assetHandle, const char *string, float scale);
	void (*Asset_Sprite_Set)(AssetHandle assetHandle, int width, int height, int marginX, int marginY);
	Image* (*Get_Img)(AssetHandle id);
	tmx_map* (*Get_TileMap)(AssetHandle id);

	unsigned int(*Snd_Play)(AssetHandle asset, float volume, float pan, bool loop);
	void(*Snd_Stop)(unsigned int handle);
	void(*Snd_PauseResume)(unsigned int handle, bool pause);
} gameImportFuncs_t;

typedef struct gameExportFuncs_s {
	void(*Init)(void *clientInfo, void *imGuiContext);
	void(*Console)(const char *line);
	void(*Frame)(double dt);
} gameExportFuncs_t;

extern gameImportFuncs_t *trap;