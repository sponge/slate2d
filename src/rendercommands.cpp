#include "rendercommands.h"
#include <nanovg.h>
#include "image.h"

extern ClientInfo inf;

const void *RB_DrawRect(const void *data) {
	const drawRectCommand_t	*cmd;

	cmd = (const drawRectCommand_t *)data;

	nvgBeginPath(inf.nvg);
	nvgRect(inf.nvg, cmd->x, cmd->y, cmd->w, cmd->h);
	nvgFillColor(inf.nvg, nvgRGBA(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]));
	nvgFill(inf.nvg);

	return (const void *)(cmd + 1);
}

void SubmitRenderCommands(renderCommandList_t * list) {
	const void *data = list->cmds;

	nvgSave(inf.nvg);
	while (1) {
		nvgRestore(inf.nvg);
		nvgSave(inf.nvg);
		switch (*(const int *)data) {
		case RC_DRAW_RECT:
			data = RB_DrawRect(data);
			break;

		case RC_END_OF_LIST:
		default:
			return;
		}
	}

}
