#include "rendercommands.h"
#include <nanovg.h>
#include "image.h"

extern ClientInfo inf;

const void *RB_SetColor(const void *data) {
	auto cmd = (const setColorCommand_t *)data;

	nvgFillColor(inf.nvg, nvgRGBA(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]));

	return (const void *)(cmd + 1);
}

const void *RB_SetTransform(const void *data) {
	auto cmd = (const setTransformCommand_t *)data;

	nvgTransform(inf.nvg, cmd->transform[0], cmd->transform[1], cmd->transform[2], cmd->transform[3], cmd->transform[4], cmd->transform[5]);

	return (const void *)(cmd + 1);
}

const void *RB_DrawRect(const void *data) {
	auto cmd = (const drawRectCommand_t *)data;

	nvgBeginPath(inf.nvg);
	nvgRect(inf.nvg, cmd->x, cmd->y, cmd->w, cmd->h);
	nvgFill(inf.nvg);

	return (const void *)(cmd + 1);
}

const void *RB_DrawText(const void *data) {
	auto cmd = (const drawTextCommand_t *)data;

	nvgSave(inf.nvg);
	nvgTextAlign(inf.nvg, cmd->align);
	nvgText(inf.nvg, cmd->x, cmd->y, cmd->text, 0);
	nvgRestore(inf.nvg);

	return (const void *)(cmd + 1);
}

void SubmitRenderCommands(renderCommandList_t * list) {
	const void *data = list->cmds;

	nvgSave(inf.nvg);
	while (1) {
		switch (*(const int *)data) {

		case RC_SET_COLOR:
			data = RB_SetColor(data);
			break;

		case RC_SET_TRANSFORM:
			data = RB_SetTransform(data);
			break;

		case RC_DRAW_RECT:
			data = RB_DrawRect(data);
			break;

		case RC_DRAW_TEXT:
			data = RB_DrawText(data);
			break;

		case RC_END_OF_LIST:
		default:
			nvgRestore(inf.nvg);
			return;
		}
	}
}
