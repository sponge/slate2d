#include "rendercommands.h"
#include <nanovg.h>
#include "assetloader.h"
#include "bitmapfont.h"

#include "console/console.h"

extern ClientInfo inf;

const void *RB_SetColor(const void *data) {
	auto cmd = (const setColorCommand_t *)data;

	if (cmd->which == COLOR_FILL) {
		nvgFillColor(inf.nvg, nvgRGBA(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3])); 
	}
	else {
		nvgStrokeColor(inf.nvg, nvgRGBA(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]));
	}

	return (const void *)(cmd + 1);
}

const void *RB_SetTransform(const void *data) {
	auto cmd = (const setTransformCommand_t *)data;

	if (cmd->absolute) {
		nvgResetTransform(inf.nvg);
	}

	nvgTransform(inf.nvg, cmd->transform[0], cmd->transform[1], cmd->transform[2], cmd->transform[3], cmd->transform[4], cmd->transform[5]);
	nvgStrokeWidth(inf.nvg, 1 / cmd->transform[0]);

	return (const void *)(cmd + 1);
}

const void *RB_SetScissor(const void *data) {
	auto cmd = (const setScissorCommand_t *)data;
	if (cmd->w <= 0 || cmd->h <= 0) {
		nvgResetScissor(inf.nvg);
	}
	else {
		nvgScissor(inf.nvg, cmd->x, cmd->y, cmd->w, cmd->h);
	}

	return (const void *)(cmd + 1);
}

const void *RB_DrawRect(const void *data) {
	auto cmd = (const drawRectCommand_t *)data;

	nvgBeginPath(inf.nvg);
	nvgRect(inf.nvg, cmd->x, cmd->y, cmd->w, cmd->h);
	if (cmd->outline) {
		nvgStroke(inf.nvg);
	}
	else {
		nvgFill(inf.nvg);
	}

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

const void *RB_DrawBmpText(const void *data) {
	auto cmd = (const drawBmpTextCommand_t *)data;

	BMPFNT_DrawText(cmd->fntId, cmd->x, cmd->y, cmd->scale, cmd->text);

	return (const void *)(cmd + 1);
}

const void *RB_DrawImage(const void *data) {
	auto cmd = (const drawImageCommand_t *)data;

	float flipX = cmd->flipBits & FLIP_H ? -1.0f : 1.0f;
	float flipY = cmd->flipBits & FLIP_V ? -1.0f : 1.0f;
	bool flipDiag = cmd->flipBits & FLIP_DIAG;

	nvgSave(inf.nvg);

	nvgTranslate(inf.nvg, cmd->x, cmd->y);

	if (flipDiag) {
		nvgTransform(inf.nvg, 0, 1, 1, 0, 0, 0);
	}

	if ((flipX == -1) ^ (flipY == -1) && flipDiag) {
		nvgScale(inf.nvg, flipY, flipX);
	}
	else {
		nvgScale(inf.nvg, flipX, flipY);
	}

	nvgTranslate(inf.nvg, -(cmd->w / 2), -(cmd->h / 2));

	auto img = Get_Img(cmd->imgId);
	auto paint = nvgImagePattern(inf.nvg, 0 - cmd->ox, 0 - cmd->oy, img->w, img->h, 0, img->hnd, cmd->alpha);
	paint.shader = cmd->shaderId;
	nvgGlobalCompositeBlendFuncSeparate(inf.nvg, NVG_SRC_ALPHA, NVG_ONE_MINUS_SRC_ALPHA, NVG_ONE, NVG_ONE_MINUS_SRC_ALPHA);
	nvgBeginPath(inf.nvg);
	nvgRect(inf.nvg, 0, 0, cmd->w, cmd->h);
	nvgFillPaint(inf.nvg, paint);
	nvgFill(inf.nvg);

	nvgRestore(inf.nvg);

	return (const void *)(cmd + 1);
}

const void *RB_DrawLine(const void *data) {
	auto cmd = (const drawLineCommand_t *)data;
	nvgBeginPath(inf.nvg);
	nvgMoveTo(inf.nvg, cmd->x1, cmd->y1);
	nvgLineTo(inf.nvg, cmd->x2, cmd->y2);
	nvgStroke(inf.nvg);

	return (const void *)(cmd + 1);
}

const void *RB_DrawCircle(const void *data) {
	auto cmd = (const drawCircleCommand_t *)data;

	nvgBeginPath(inf.nvg);
	nvgCircle(inf.nvg, cmd->x, cmd->y, cmd->radius);
	if (cmd->outline) {
		nvgStroke(inf.nvg);
	}
	else {
		nvgFill(inf.nvg);
	}

	return (const void *)(cmd + 1);
}

const void *RB_DrawTri(const void *data) {
	auto cmd = (const drawTriCommand_t *)data;

	nvgBeginPath(inf.nvg);
	nvgMoveTo(inf.nvg, cmd->x1, cmd->y1);
	nvgLineTo(inf.nvg, cmd->x2, cmd->y2);
	nvgLineTo(inf.nvg, cmd->x3, cmd->y3);
	nvgLineTo(inf.nvg, cmd->x1, cmd->y1);
	
	if (cmd->outline) {
		nvgStroke(inf.nvg);
	}
	else {
		nvgFill(inf.nvg);
	}

	return (const void *)(cmd + 1);
}

void SubmitRenderCommands(renderCommandList_t * list) {
	const void *data = list->cmds;

	nvgSave(inf.nvg);
	while (1) {
		switch (*(const byte *)data) {

		case RC_SET_COLOR:
			data = RB_SetColor(data);
			break;

		case RC_SET_TRANSFORM:
			data = RB_SetTransform(data);
			break;

		case RC_SET_SCISSOR:
			data = RB_SetScissor(data);
			break;

		case RC_DRAW_RECT:
			data = RB_DrawRect(data);
			break;

		case RC_DRAW_TEXT:
			data = RB_DrawText(data);
			break;

		case RC_DRAW_BMPTEXT:
			data = RB_DrawBmpText(data);
			break;

		case RC_DRAW_IMAGE:
			data = RB_DrawImage(data);
			break;

		case RC_DRAW_LINE:
			data = RB_DrawLine(data);
			break;

		case RC_DRAW_CIRCLE:
			data = RB_DrawCircle(data);
			break;

		case RC_DRAW_TRI:
			data = RB_DrawTri(data);
			break;

		case RC_END_OF_LIST:
			nvgRestore(inf.nvg);
			return;

		default:
			Com_Error(ERR_FATAL, "Bad render command byte id %i", *(const int *)data);
			return;
		}
	}
}
