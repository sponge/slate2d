#pragma once
#include <tmx.h>

#include "shared.h"

typedef struct gameImportFuncs_s {
	void (*SendConsoleCommand)(const char * text);
	void (*Print)(const char *fmt, ...);
	void (*Error)(int level, const char *error, ...);
	void (*SetWindowTitle)(const char *title);

	conVar_t* (*Con_GetVarDefault)(const char *var_name, const char *var_value, int flags);
	conVar_t* (*Con_GetVar)(const char * name);
	conVar_t* (*Con_SetVar)(const char *var_name, const char *value);

	int (*Con_GetArgCount)(void);
	const char * (*Con_GetArg)(int arg);
	const char * (*Con_GetArgs)(int start);

	int  (*FS_ReadFile)(const char *path, void **buffer);
	bool (*FS_Exists)(const char *file);
	char** (*FS_List)(const char *path);
	void (*FS_FreeList)(void * listVar);

	void(*IN_AllocateButtons)(const char **buttonNames, int buttonCount);
	buttonState_t*(*IN_GetButton)(int buttonNum);
	bool (*IN_ButtonPressed)(int buttonId, unsigned int delay, int repeat);
	MousePosition (*IN_MousePosition)();

	void(*SubmitRenderCommands)(renderCommandList_t *list);

	AssetHandle (*Asset_Create)(AssetType_t assetType, const char *name, const char *path, int flags);
	AssetHandle (*Asset_Find)(const char *name);
	void (*Asset_Load)(AssetHandle assetHandle);
	void (*Asset_LoadAll)();
	void (*Asset_ClearAll)();
	void (*Asset_BMPFNT_Set)(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);
	int (*Asset_TextWidth)(AssetHandle assetHandle, const char *string, float scale);
	void (*Asset_Sprite_Set)(AssetHandle assetHandle, int width, int height, int marginX, int marginY);
	void (*Asset_Canvas_Set)(AssetHandle assetHandle, int width, int height);
	void (*Asset_Shader_Set)(AssetHandle id, bool isFile, char *vs, char *fs);
	Image* (*Get_Img)(AssetHandle id);
	tmx_map* (*Get_TileMap)(AssetHandle id);

	unsigned int(*Snd_Play)(AssetHandle asset, float volume, float pan, bool loop);
	void(*Snd_Stop)(unsigned int handle);
	void(*Snd_PauseResume)(unsigned int handle, bool pause);
} gameImportFuncs_t;

typedef struct gameExportFuncs_s {
	void(*Init)(void *clientInfo, void *imGuiContext);
	bool(*Console)();
	void(*Frame)(double dt);
	void(*Error)(int level, const char *msg);
} gameExportFuncs_t;

extern gameImportFuncs_t *trap;