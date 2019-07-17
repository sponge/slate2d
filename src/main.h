#pragma once
#include <cstdint>

#include "shared.h"

#ifndef NOTUSED
#define NOTUSED(v) for (;;) { (void)(1 ? (void)0 : ( (void)(v) ) ); break; }
#endif

extern bool consoleActive;

extern int64_t last_update_musec, com_frameTime, frame_musec;

void DropToMenu();
void SetWindowTitle(const char *title);

#define SLT_API extern "C" __declspec(dllexport)

SLT_API void SLT_Init(int argc, char* argv[]);
SLT_API void SLT_Shutdown();

SLT_API void SLT_SendConsoleCommand(const char* text);
SLT_API void SLT_Print(const char* fmt, ...);
SLT_API void SLT_Error(int level, const char* error, ...);
SLT_API void SLT_SetWindowTitle(const char* title);

SLT_API conVar_t* SLT_Con_GetVarDefault(const char* var_name, const char* var_value, int flags);
SLT_API conVar_t* SLT_Con_GetVar(const char* name);
SLT_API conVar_t* SLT_Con_SetVar(const char* var_name, const char* value);

SLT_API int SLT_Con_GetArgCount(void);
SLT_API const char* SLT_Con_GetArg(int arg);
SLT_API const char* SLT_Con_GetArgs(int start);

SLT_API int SLT_FS_ReadFile(const char* path, void** buffer);
SLT_API bool SLT_FS_Exists(const char* file);
SLT_API char** SLT_FS_List(const char* path);
SLT_API void SLT_FS_FreeList(void* listVar);

SLT_API void SLT_In_AllocateButtons(const char** buttonNames, int buttonCount);
SLT_API buttonState_t* SLT_In_GetButton(int buttonNum);
SLT_API bool SLT_In_ButtonPressed(int buttonId, unsigned int delay, int repeat);
SLT_API MousePosition SLT_In_MousePosition();

SLT_API void SLT_SubmitRenderCommands(renderCommandList_t* list);

SLT_API AssetHandle SLT_Asset_Create(AssetType_t assetType, const char* name, const char* path, int flags);
SLT_API AssetHandle SLT_Asset_Find(const char* name);
SLT_API void SLT_Asset_Load(AssetHandle assetHandle);
SLT_API void SLT_Asset_LoadAll();
SLT_API void SLT_Asset_ClearAll();
SLT_API void SLT_Asset_LoadINI(const char* path);
SLT_API void SLT_Asset_BMPFNT_Set(AssetHandle assetHandle, const char* glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);
SLT_API int SLT_Asset_TextWidth(AssetHandle assetHandle, const char* string, float scale);
SLT_API const char* SLT_Asset_BreakString(int width, const char* in);
SLT_API void SLT_Asset_Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY);
SLT_API void SLT_Asset_Canvas_Set(AssetHandle assetHandle, int width, int height);
SLT_API void SLT_Asset_Shader_Set(AssetHandle id, bool isFile, const char* vs, const char* fs);
SLT_API Image* SLT_Get_Img(AssetHandle id);
SLT_API tmx_map* SLT_Get_TileMap(AssetHandle id);

SLT_API unsigned int SLT_Snd_Play(AssetHandle asset, float volume, float pan, bool loop);
SLT_API void SLT_Snd_Stop(unsigned int handle);
SLT_API void SLT_Snd_PauseResume(unsigned int handle, bool pause);