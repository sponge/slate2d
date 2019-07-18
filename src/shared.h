#pragma once
// shared.h is shared between the engine and the dll

#include <tmx.h>
#include <stdint.h>
#include "../src/console.h"

#ifdef _MSC_VER 
#define STRFUNCS
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

const char * __cdecl tempstr(const char *format, ...);

typedef unsigned char byte;

#ifndef ERR_NONE
#define ERR_NONE 0
#define ERR_GAME 1
#define ERR_FATAL 2
#endif

typedef struct {
	int width, height;
} ClientInfo;

// RENDER COMMANDS

#define	MAX_RENDER_COMMANDS	0x40000

#define IMAGEFLAGS_LINEAR_FILTER 1 << 0

typedef struct {
	byte	cmds[MAX_RENDER_COMMANDS];
	int		used;
} renderCommandList_t;

typedef struct {
	byte	commandId;
	byte	color[4];
} setColorCommand_t;

typedef struct {
	byte commandId;
	byte	color[4];
} clearCommand_t;

typedef struct {
	byte commandId;
} resetTransformCommand_t;

typedef struct {
	byte	commandId;
	float	x, y;
} scaleCommand_t;

typedef struct {
	byte	commandId;
	float	angle;
} rotateCommand_t;

typedef struct {
	byte	commandId;
	float	x;
	float	y;
} translateCommand_t;

typedef struct {
	byte commandId;
	int x, y, w, h;
} setScissorCommand_t;

typedef struct {
	byte	commandId;
	unsigned int canvasId;
} useCanvasCommand_t;

typedef struct {
	byte	commandId;
} resetCanvasCommand_t;

typedef struct {
	byte	commandId;
	unsigned int shaderId;
} useShaderCommand_t;

typedef struct {
	byte	commandId;
} resetShaderCommand_t;

typedef struct {
	byte	commandId;
	byte	outline;
	float	x, y, w, h;
} drawRectCommand_t;

typedef struct {
	byte	commandId;
	unsigned int fntId;
	float size;
	float lineHeight;
	int align;
} setTextStyleCommand_t;

// strSz is the size of the string. the actual string lives
// in the buffer right after this command so it can be any
// reasonable size. len is separate here so the engine can
// parse out color codes without the game code needing to
// understand them.
typedef struct {
	byte commandId;
	float x, y, w, len;
	unsigned int strSz;
} drawTextCommand_t;

#define FLIP_H 1
#define FLIP_V 2
#define FLIP_DIAG 4

typedef struct {
	byte commandId;
	float x, y, w, h, ox, oy, angle, scale;
	byte flipBits;
	unsigned int imgId;
} drawImageCommand_t;

typedef struct {
	byte commandId;
	unsigned int spr;
	int id;
	float x, y;
	float scale;
	byte flipBits;
	int w, h;
} drawSpriteCommand_t;

typedef struct {
	byte commandId;
	float x1, y1, x2, y2;
} drawLineCommand_t;

typedef struct {
	byte	commandId;
	byte	outline;
	float x, y, radius;
} drawCircleCommand_t;

typedef struct {
	byte	commandId;
	byte	outline;
	float x1, y1, x2, y2, x3, y3;
} drawTriCommand_t;

typedef struct {
	byte commandId;
	float x, y;
	unsigned int mapId;
	unsigned int layer, cellX, cellY, cellW, cellH;
} drawMapCommand_t;

typedef enum {
	RC_END_OF_LIST,
	RC_SET_COLOR,
	RC_CLEAR,
	RC_SET_TEXT_STYLE,
	RC_RESET_TRANSFORM,
	RC_SCALE,
	RC_ROTATE,
	RC_TRANSLATE,
	RC_SET_SCISSOR,
	RC_USE_CANVAS,
	RC_RESET_CANVAS,
	RC_USE_SHADER,
	RC_RESET_SHADER,
	RC_DRAW_RECT,
	RC_DRAW_TEXT,
	RC_DRAW_IMAGE,
	RC_DRAW_SPRITE,
	RC_DRAW_LINE,
	RC_DRAW_CIRCLE,
	RC_DRAW_TRI,
	RC_DRAW_MAP_LAYER,
} renderCommand_t;

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
	AssetHandle id;
	AssetType_t type;
	bool loaded;
	const char *name;
	const char *path;
	int flags;
	void *resource;
} Asset;

typedef struct {
	unsigned int hnd;
	int w, h;
} Image;

typedef struct {
	Image *texture;
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
	int16_t framex;
	int16_t framey;
	int16_t framew;
	int16_t frameh;
	byte rotated;
} Sprite;

typedef struct {
	int numImages;
	int numSprites;
	Image *images;
	Sprite *sprites;

	int staticWidth;
	int staticHeight;
	int staticMarginX;
	int staticMarginY;
} SpriteAtlas;

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