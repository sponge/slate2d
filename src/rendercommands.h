#pragma once
#include "assetloader.h"
#include "external/fontstash.h"

#define	MAX_RENDER_COMMANDS	0x50000

typedef struct {
	uint8_t	cmds[MAX_RENDER_COMMANDS];
	int		used;
} renderCommandList_t;

typedef struct {
	uint8_t	commandId;
	uint8_t	color[4];
} setColorCommand_t;

typedef struct {
	uint8_t commandId;
	uint8_t	color[4];
} clearCommand_t;

typedef struct {
	uint8_t commandId;
} resetTransformCommand_t;

typedef struct {
	uint8_t	commandId;
	float	x, y;
} scaleCommand_t;

typedef struct {
	uint8_t	commandId;
	float	angle;
} rotateCommand_t;

typedef struct {
	uint8_t	commandId;
	float	x;
	float	y;
} translateCommand_t;

typedef struct {
	uint8_t commandId;
	int x, y, w, h;
} setScissorCommand_t;

typedef struct {
	uint8_t	commandId;
	unsigned int canvasId;
} useCanvasCommand_t;

typedef struct {
	uint8_t	commandId;
} resetCanvasCommand_t;

typedef struct {
	uint8_t	commandId;
	unsigned int shaderId;
} useShaderCommand_t;

typedef struct {
	uint8_t	commandId;
} resetShaderCommand_t;

typedef struct {
	uint8_t	commandId;
	uint8_t	outline;
	float	x, y, w, h;
} drawRectCommand_t;

typedef struct {
	uint8_t	commandId;
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
	uint8_t commandId;
	float x, y, w, len;
	unsigned int strSz;
} drawTextCommand_t;

typedef struct {
	uint8_t commandId;
	float x, y, w, h, ox, oy, angle, scale;
	uint8_t flipBits;
	unsigned int imgId;
} drawImageCommand_t;

typedef struct {
	uint8_t commandId;
	unsigned int spr;
	int id;
	float x, y;
	float scale;
	uint8_t flipBits;
	int w, h;
} drawSpriteCommand_t;

typedef struct {
	uint8_t commandId;
	float x1, y1, x2, y2;
} drawLineCommand_t;

typedef struct {
	uint8_t	commandId;
	uint8_t	outline;
	float x, y, radius;
} drawCircleCommand_t;

typedef struct {
	uint8_t	commandId;
	uint8_t	outline;
	float x1, y1, x2, y2, x3, y3;
} drawTriCommand_t;

typedef struct {
	uint8_t commandId;
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

void SubmitRenderCommands(renderCommandList_t *list);
void DrawImage(float x, float y, float w, float h, float ox, float oy, float scale, uint8_t flipBits, unsigned int handle, int imgW, int imgH);

struct RenderState {
	uint8_t color[4] = { 255, 255, 255, 255 };
	int align = 1;
	float lineHeight = 1.0f;
};
typedef struct RenderState RenderState;

extern RenderState state;
extern FONScontext *ctx;