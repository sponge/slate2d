#pragma once
// shared.h is shared between the dll and the exe

#include <tmx.h>
#include <stdint.h>
#include "../src/console.h"

const char * __cdecl tempstr(const char *format, ...);

#ifndef ERR_NONE
#define ERR_NONE 0
#define ERR_GAME 1
#define ERR_FATAL 2
#endif

typedef struct {
	int width, height;
} ClientInfo;

#define IMAGEFLAGS_LINEAR_FILTER 1 << 0

#define FLIP_H 1
#define FLIP_V 2
#define FLIP_DIAG 4

// ASSETS

typedef int AssetHandle;

#define INVALID_ASSET -1

typedef enum {
	ASSET_ANY,
	ASSET_IMAGE,
	ASSET_SPRITE,
	ASSET_SPEECH,
	ASSET_SOUND,
	ASSET_MOD,
	ASSET_FONT,
	ASSET_BITMAPFONT,
	ASSET_TILEMAP,
	ASSET_CANVAS,
	ASSET_SHADER,
	ASSET_MAX
} AssetType_t;

static const char* assetStrings[] = {
	"ASSET_ANY",
	"ASSET_IMAGE",
	"ASSET_SPRITE",
	"ASSET_SPEECH",
	"ASSET_SOUND",
	"ASSET_MOD",
	"ASSET_FONT",
	"ASSET_BITMAPFONT",
	"ASSET_TILEMAP",
	"ASSET_CANVAS",
	"ASSET_SHADER",
	"ASSET_MAX",
};

typedef struct {
	unsigned int hnd;
	int w, h;
} Image;

typedef struct {
	int x, y;
} MousePosition;

#ifdef _MSC_VER 
	#ifdef COMPILE_DLL
	#define SLT_API extern "C" __declspec(dllexport)
	#else
	#define SLT_API extern "C" __declspec(dllimport)
	#endif
#else
	#ifdef COMPILE_DLL
		#define SLT_API extern "C" __attribute__ ((visibility ("default")))
	#else
		#define SLT_API extern "C"
	#endif
#endif

SLT_API void SLT_Init(int argc, char* argv[]);
SLT_API void SLT_Shutdown();
SLT_API void SLT_Con_SetErrorHandler(void(*errHandler)(int level, const char *msg));

SLT_API double SLT_StartFrame();
SLT_API void SLT_EndFrame();

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
SLT_API void SLT_Con_AddCommand(const char* name, conCmd_t cmd);

SLT_API int SLT_FS_ReadFile(const char* path, void** buffer);
SLT_API bool SLT_FS_Exists(const char* file);
SLT_API char** SLT_FS_List(const char* path);
SLT_API void SLT_FS_FreeList(void* listVar);

SLT_API void SLT_In_AllocateButtons(const char** buttonNames, int buttonCount);
SLT_API buttonState_t* SLT_In_GetButton(int buttonNum);
SLT_API bool SLT_In_ButtonPressed(int buttonId, unsigned int delay, int repeat);
SLT_API MousePosition SLT_In_MousePosition();

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

SLT_API const void* SLT_GetClientInfo();
SLT_API const void* SLT_GetImguiContext();

SLT_API void SLT_UpdateLastFrameTime();

SLT_API void DC_SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
SLT_API void DC_ResetTransform();
SLT_API void DC_Scale(float x, float y);
SLT_API void DC_Rotate(float angle);
SLT_API void DC_Translate(float x, float y);
SLT_API void DC_SetScissor(int x, int y, int w, int h);
SLT_API void DC_ResetScissor();
SLT_API void DC_UseCanvas(unsigned int canvasId);
SLT_API void DC_ResetCanvas();
SLT_API void DC_UseShader(unsigned int shaderId);
SLT_API void DC_ResetShader();
SLT_API void DC_DrawRect(float x, float y, float w, float h, bool outline = false);
SLT_API void DC_SetTextStyle(unsigned int fntId, float size, float lineHeight, int align);
SLT_API void DC_DrawText(float x, float y, float h, const char* text, int len);
SLT_API void DC_DrawImage(unsigned int imgId, float x, float y, float w = 0.0f, float h = 0.0f, float scale = 1.0f, uint8_t flipBits = 0, float ox = 0.0f, float oy = 0.0f);
SLT_API void DC_DrawSprite(unsigned int sprite, int id, float x, float y, float scale = 1.0f, uint8_t flipBits = 0, int w = 1, int h = 1);
SLT_API void DC_DrawLine(float x1, float y1, float x2, float y2);
SLT_API void DC_DrawCircle(float x, float y, float radius, bool outline = false);
SLT_API void DC_DrawTri(float x1, float y1, float x2, float y2, float x3, float y3, bool outline = false);
SLT_API void DC_DrawMapLayer(unsigned int mapId, unsigned int layer, float x = 0, float y = 0, unsigned int cellX = 0, unsigned int cellY = 0, unsigned int cellW = 0, unsigned int cellH = 0);
SLT_API void DC_Submit();
SLT_API void DC_Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);