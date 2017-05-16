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

void DC_DrawRect(float x, float y, float w, float h, int r, int g, int b, int a) {
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
	cmd->color[0] = r;
	cmd->color[1] = g;
	cmd->color[2] = b;
	cmd->color[3] = a;

}