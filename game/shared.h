#pragma once
// types that are shared across the dll and engine
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

// SCENE

class Scene {
public:
	virtual ~Scene() {};
	virtual void Startup(ClientInfo* i) = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Console(const char *line) = 0;
};

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
	float x, y, w, h;
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
// reasonable size.
typedef struct {
	byte commandId;
	float x, y, w;
	unsigned int strSz;
} drawTextCommand_t;

#define FLIP_H 1
#define FLIP_V 2
#define FLIP_DIAG 4

typedef struct {
	byte commandId;
	float x, y, w, h, ox, oy, angle, alpha, scale;
	byte flipBits;
	unsigned int imgId;
} drawImageCommand_t;

typedef struct {
	byte commandId;
	unsigned int spr;
	int id;
	float x, y;
	float alpha;
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
	char name[64];
	char path[64];
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