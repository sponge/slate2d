#include "public.h"

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

void DC_SetColor(byte color[4]) {
	setColorCommand_t *cmd;

	cmd = (setColorCommand_t *)R_GetCommandBuffer(sizeof(*cmd));
	if (!cmd) {
		return;
	}

	cmd->commandId = RC_SET_COLOR;
	cmd->color[0] = color[0];
	cmd->color[1] = color[1];
	cmd->color[2] = color[2];
	cmd->color[3] = color[3];
}

void DC_SetColor(byte r, byte g, byte b, byte a) {
	setColorCommand_t *cmd;

	cmd = (setColorCommand_t *)R_GetCommandBuffer(sizeof(*cmd));
	if (!cmd) {
		return;
	}

	cmd->commandId = RC_SET_COLOR;
	cmd->color[0] = r;
	cmd->color[1] = g;
	cmd->color[2] = b;
	cmd->color[3] = a;
}

void DC_SetTransform(float a, float b, float c, float d, float e, float f) {
	setTransformCommand_t *cmd;

	cmd = (setTransformCommand_t *)R_GetCommandBuffer(sizeof(*cmd));
	if (!cmd) {
		return;
	}

	cmd->commandId = RC_SET_TRANSFORM;
	cmd->transform[0] = a;
	cmd->transform[1] = b;
	cmd->transform[2] = c;
	cmd->transform[3] = d;
	cmd->transform[4] = e;
	cmd->transform[5] = f;
}

void DC_DrawRect(float x, float y, float w, float h) {
	drawRectCommand_t *cmd;

	cmd = (drawRectCommand_t *) R_GetCommandBuffer(sizeof(*cmd));
	if (!cmd) {
		return;
	}

	cmd->commandId = RC_DRAW_RECT;
	cmd->x = x;
	cmd->y = y;
	cmd->w = w;
	cmd->h = h;
}

void DC_DrawText(float x, float y, const char *text, int align) {
	drawTextCommand_t *cmd;

	cmd = (drawTextCommand_t *)R_GetCommandBuffer(sizeof(*cmd));
	if (!cmd) {
		return;
	}

	cmd->commandId = RC_DRAW_TEXT;
	strncpy(&cmd->text[0], text, sizeof(cmd->text));
	cmd->align = align;
	cmd->x = x;
	cmd->y = y;
}