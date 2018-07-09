#include "public.h"
#include "draw.h"
#include <assert.h>
#include <cstring>

#define GET_COMMAND(type, id) type *cmd; cmd = (type *)R_GetCommandBuffer(sizeof(*cmd)); if (!cmd) { return; } cmd->commandId = id;

// FIXME: wrap me in a class instead?

static renderCommandList_t cmdList;

void *R_GetCommandBuffer(int bytes) {
	// always leave room for the end of list command
	if (cmdList.used + bytes + 4 > MAX_RENDER_COMMANDS) {
		if (bytes > MAX_RENDER_COMMANDS - 4) {
			trap->Error(ERR_FATAL, "R_GetCommandBuffer: bad size %i", bytes);
		}
		// if we run out of room, just start dropping commands
		return NULL;
	}

	cmdList.used += bytes;

	return cmdList.cmds + cmdList.used - bytes;
}

void DC_Submit() {
	trap->SubmitRenderCommands(&cmdList);
}

void DC_Clear() {
	memset(&cmdList, 0, sizeof(cmdList));
}

void DC_SetColor(byte which, byte r, byte g, byte b, byte a) {
	GET_COMMAND(setColorCommand_t, RC_SET_COLOR)
	cmd->which = which;
	cmd->color[0] = r;
	cmd->color[1] = g;
	cmd->color[2] = b;
	cmd->color[3] = a;
}

void DC_SetColor(byte which, byte color[4]) {
	DC_SetColor(which, color[0], color[1], color[2], color[3]);
}

void DC_ResetTransform() {
	GET_COMMAND(resetTransformCommand_t, RC_RESET_TRANSFORM)
}

void DC_Transform(float a, float b, float c, float d, float e, float f) {
	GET_COMMAND(transformCommand_t, RC_TRANSFORM)
	cmd->transform[0] = a;
	cmd->transform[1] = b;
	cmd->transform[2] = c;
	cmd->transform[3] = d;
	cmd->transform[4] = e;
	cmd->transform[5] = f;
}

void DC_Rotate(float angle) {
	GET_COMMAND(rotateCommand_t, RC_ROTATE);
	cmd->angle = angle;
}

void DC_Translate(float x, float y) {
	GET_COMMAND(translateCommand_t, RC_TRANSLATE);
	cmd->x = x;
	cmd->y = y;
}

void DC_SetScissor(float x, float y, float w, float h) {
	GET_COMMAND(setScissorCommand_t, RC_SET_SCISSOR)
	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->h = h;
}

void DC_ResetScissor() {
	DC_SetScissor(0, 0, 0, 0);
}

void DC_DrawRect(float x, float y, float w, float h, bool outline) {
	GET_COMMAND(drawRectCommand_t, RC_DRAW_RECT)
	cmd->outline = outline;
	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->h = h;
}

void DC_SetTextStyle(unsigned int fntId, unsigned int size, float lineHeight) {
	GET_COMMAND(setTextStyleCommand_t, RC_SET_TEXT_STYLE)

	cmd->fntId = fntId;
	cmd->size = size;
	cmd->lineHeight = lineHeight;
}

void DC_DrawText(float x, float y, float w, const char *text) {
	GET_COMMAND(drawTextCommand_t, RC_DRAW_TEXT)

	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->strSz = strlen(text) + 1;

	void *strStart = R_GetCommandBuffer(cmd->strSz);
	strncpy((char*)strStart, text, strlen(text));
}

void DC_DrawBmpText(unsigned int fntId, float x, float y, const char *text, float scale) {
	GET_COMMAND(drawBmpTextCommand_t, RC_DRAW_BMPTEXT)

	cmd->fntId = fntId;
	cmd->x = x;
	cmd->y = y;
	cmd->scale = scale;
	cmd->strSz = strlen(text) + 1;

	void *strStart = R_GetCommandBuffer(cmd->strSz);
	strncpy((char*)strStart, text, strlen(text));
}

void DC_DrawImage(unsigned int imgId, float x, float y, float w, float h, float alpha, float scale, byte flipBits, float ox, float oy, unsigned int shaderId) {
	GET_COMMAND(drawImageCommand_t, RC_DRAW_IMAGE)
	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->h = h;
	cmd->ox = ox;
	cmd->oy = oy;
	cmd->alpha = alpha;
	cmd->scale = scale;
	cmd->flipBits = flipBits;
	cmd->imgId = imgId;
	cmd->shaderId = shaderId;
}

void DC_DrawSprite(unsigned int spr, int id, float x, float y, float alpha, float scale, byte flipBits, int w, int h) {
	GET_COMMAND(drawSpriteCommand_t, RC_DRAW_SPRITE);
	cmd->spr = spr;
	cmd->id = id;
	cmd->x = x;
	cmd->y = y;
	cmd->alpha = alpha;
	cmd->scale = scale;
	cmd->flipBits = flipBits;
	cmd->w = w;
	cmd->h = h;
}

void DC_DrawLine(float x1, float y1, float x2, float y2) {
	GET_COMMAND(drawLineCommand_t, RC_DRAW_LINE);
	cmd->x1 = x1;
	cmd->y1 = y1;
	cmd->x2 = x2;
	cmd->y2 = y2;
}

void DC_DrawCircle(float x, float y, float radius, bool outline) {
	GET_COMMAND(drawCircleCommand_t, RC_DRAW_CIRCLE);
	cmd->outline = outline;
	cmd->x = x;
	cmd->y = y;
	cmd->radius = radius;
}

void DC_DrawTri(float x1, float y1, float x2, float y2, float x3, float y3, bool outline) {
	GET_COMMAND(drawTriCommand_t, RC_DRAW_TRI);
	cmd->outline = outline;
	cmd->x1 = x1;
	cmd->y1 = y1;
	cmd->x2 = x2;
	cmd->y2 = y2;
	cmd->x3 = x3;
	cmd->y3 = y3;
}

void DC_DrawMapLayer(unsigned int layer, float x, float y, unsigned int cellX, unsigned int cellY, unsigned int cellW, unsigned int cellH)
{
	GET_COMMAND(drawMapCommand_t, RC_DRAW_MAP_LAYER);
	cmd->layer = layer;
	cmd->x = x;
	cmd->y = y;
	cmd->cellX = cellX;
	cmd->cellY = cellY;
	cmd->cellW = cellW;
	cmd->cellH = cellH;
}