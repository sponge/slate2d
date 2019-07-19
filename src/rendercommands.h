#pragma once
#include "shared.h"

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

void SubmitRenderCommands(renderCommandList_t *list);
void DrawImage(float x, float y, float w, float h, float ox, float oy, float scale, byte flipBits, unsigned int handle, int imgW, int imgH);

struct RenderState {
	byte color[4] = { 255, 255, 255, 255 };
	// FIXME: are these 4 fields still necessary?
	Asset *font;
	int align = 1;
	float lineHeight = 1.0f;
	float size = 1.0f;
};
typedef struct RenderState RenderState;

extern RenderState state;