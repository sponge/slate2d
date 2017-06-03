#pragma once
#include <nanovg.h>
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
	Img* (*Img_Create)(const char * name, const char * path);
	void (*Img_Load)(Img & img);
	void (*Img_LoadAll)();
	Img* (*Img_Find)(const char * name);
	Img* (*Img_Get)(unsigned int i);
	bool (*Img_Free)(const char * name);
	BitmapFont * (*BMPFNT_Create)(const char *name, const char *path, const char *glyphs, int charSpacing, int spaceWidth, int lineHeight);
	void (*BMPFNT_Load)(BitmapFont &font);
	BitmapFont * (*BMPFNT_Get)(unsigned int i);
	BitmapFont * (*BMPFNT_Find)(const char *name);
	int (*BMPFNT_TextWidth)(BitmapFont &font, const char *string);
	void (*BMPFNT_LoadAll)();
	bool (*BMPFNT_Free)(const char *name);
	void (*Scene_Switch)(Scene *newScene);
	Scene* (*Scene_Get)(int i);
	void (*Scene_Replace)(int i, Scene *newScene);
	Scene* (*Scene_Current)();
	tmx_map* (*Map_Load)(const char *file);
	void (*Map_Free)(tmx_map* map);
	void (*IN_KeyDown)(kbutton_t *b);
	void (*IN_KeyUp)(kbutton_t *b);
	float (*CL_KeyState)(kbutton_t *key);
	void(*SND_PlaySpeech)(const char *text);
	void(*SND_PlayMusic)(const char *file);
	void(*SND_PlaySound)(const char *file);
	void(*SubmitRenderCommands)(renderCommandList_t *list);
} gameImportFuncs_t;

typedef struct gameExportFuncs_s {
	void(*Init)(void *clientInfo, void *imGuiContext);
	void(*Console)(const char *line);
	void(*Frame)(float dt);
} gameExportFuncs_t;

extern gameImportFuncs_t *trap;