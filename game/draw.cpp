#include "public.h"

#define GET_COMMAND(type) type *cmd; cmd = (type *)R_GetCommandBuffer(sizeof(*cmd)); if (!cmd) { return; }

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

void DC_SetColor(byte r, byte g, byte b, byte a) {
	GET_COMMAND(setColorCommand_t)
	cmd->commandId = RC_SET_COLOR;
	cmd->color[0] = r;
	cmd->color[1] = g;
	cmd->color[2] = b;
	cmd->color[3] = a;
}

void DC_SetColor(byte color[4]) {
	DC_SetColor(color[0], color[1], color[2], color[3]);
}

void DC_SetTransform(bool absolute, float a, float b, float c, float d, float e, float f) {
	GET_COMMAND(setTransformCommand_t)
	cmd->commandId = RC_SET_TRANSFORM;
	cmd->absolute = absolute;
	cmd->transform[0] = a;
	cmd->transform[1] = b;
	cmd->transform[2] = c;
	cmd->transform[3] = d;
	cmd->transform[4] = e;
	cmd->transform[5] = f;
}

void DC_DrawRect(float x, float y, float w, float h) {
	GET_COMMAND(drawRectCommand_t)
	cmd->commandId = RC_DRAW_RECT;
	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->h = h;
}

void DC_DrawText(float x, float y, const char *text, int align) {
	GET_COMMAND(drawTextCommand_t)
	cmd->commandId = RC_DRAW_TEXT;
	strncpy(&cmd->text[0], text, sizeof(cmd->text));
	cmd->align = align;
	cmd->x = x;
	cmd->y = y;
}

void DC_DrawBmpText(float x, float y, float scale, const char *text, unsigned int fntId) {
	GET_COMMAND(drawBmpTextCommand_t)
	cmd->commandId = RC_DRAW_BMPTEXT;
	cmd->fntId = fntId;
	strncpy(&cmd->text[0], text, sizeof(cmd->text));
	cmd->x = x;
	cmd->y = y;
	cmd->scale = scale;
}

void DC_DrawImage(float x, float y, float w, float h, float ox, float oy, float alpha, byte flipBits, unsigned int imgId, unsigned int shaderId) {
	GET_COMMAND(drawImageCommand_t)
	cmd->commandId = RC_DRAW_IMAGE;
	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->h = h;
	cmd->ox = ox;
	cmd->oy = oy;
	cmd->alpha = alpha;
	cmd->flipBits = flipBits;
	cmd->imgId = imgId;
	cmd->shaderId = shaderId;
}