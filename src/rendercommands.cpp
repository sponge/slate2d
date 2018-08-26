#include "rendercommands.h"
#include <algorithm>
#include <tmx.h>
#include "assetloader.h"
#include "bitmapfont.h"
#include "rlgl.h"

#include "console/console.h"

extern ClientInfo inf;

float currentColor[4] = { 1.0, 1.0, 1.0, 1.0 };

const void *RB_SetColor(const void *data) {
	auto cmd = (const setColorCommand_t *)data;

	currentColor[0] = cmd->color[0];
	currentColor[1] = cmd->color[1];
	currentColor[2] = cmd->color[2];
	currentColor[3] = cmd->color[3];

	return (const void *)(cmd + 1);
}

const void *RB_ResetTransform(const void *data) {
	auto cmd = (const resetTransformCommand_t *)data;

	rlLoadIdentity();

	return (const void *)(cmd + 1);
}

const void *RB_Transform(const void *data) {
	auto cmd = (const transformCommand_t *)data;

	rlScalef(cmd->transform[0], cmd->transform[3], 1.0f);
	rlTranslatef(cmd->transform[4], cmd->transform[5], 0.0f);
	// FIXME: skew, we don't use it yet

	return (const void *)(cmd + 1);
}

const void *RB_Rotate(const void *data) {
	auto cmd = (const rotateCommand_t *)data;

	rlRotatef(cmd->angle, 0, 0, 0);

	return (const void *)(cmd + 1);
}

const void *RB_Translate(const void *data) {
	auto cmd = (const translateCommand_t *)data;

	rlTranslatef(cmd->x, cmd->y, 0);

	return (const void *)(cmd + 1);
}

const void *RB_SetScissor(const void *data) {
	auto cmd = (const setScissorCommand_t *)data;

	//if (cmd->w <= 0 || cmd->h <= 0) {
	//	nvgResetScissor(inf.nvg);
	//}
	//else {
	//	nvgScissor(inf.nvg, cmd->x, cmd->y, cmd->w, cmd->h);
	//}

	return (const void *)(cmd + 1);
}

const void DrawRectangle(float x, float y, float w, float h) {
	rlNormal3f(0.0f, 0.0f, 1.0f);

	rlTexCoord2f(0.0f, 0.0f);
	rlVertex2f(x, y);

	rlTexCoord2f(0.0f, 1.0f);
	rlVertex2f(x, y + h);

	rlTexCoord2f(1.0f, 1.0f);
	rlVertex2f(x + w, y + h);

	rlTexCoord2f(1.0f, 0.0f);
	rlVertex2f(x + w, y);
}

const void *RB_DrawRect(const void *data) {
	auto cmd = (const drawRectCommand_t *)data;

	rlBegin(RL_QUADS);
	rlEnableTexture(GetTextureDefault().id);
	rlColor4ub(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);

	if (cmd->outline) {
		DrawRectangle(cmd->x, cmd->y, cmd->w, 1);
		DrawRectangle(cmd->x + cmd->w - 1, cmd->y + 1, 1, cmd->h - 2);
		DrawRectangle(cmd->x, cmd->y + cmd->h - 1, cmd->w, 1);
		DrawRectangle(cmd->x, cmd->y + 1, 1, cmd->h - 2);
	}
	else {
		DrawRectangle(cmd->x, cmd->y, cmd->w, cmd->h);
	}

	rlDisableTexture();
	rlEnd();


	return (const void *)(cmd + 1);
}

const void *RB_SetTextStyle(const void *data) {
	auto cmd = (const setTextStyleCommand_t *)data;

	Asset *asset = Asset_Get(ASSET_FONT, cmd->fntId);

	assert(asset != nullptr);

	// -1 since we +1 when loading fonts to avoid returning a nullptr
	int font = (int)asset->resource - 1;

	//nvgFontFaceId(inf.nvg, font);
	//nvgFontSize(inf.nvg, cmd->size);
	//nvgTextAlign(inf.nvg, cmd->align);
	//nvgTextLineHeight(inf.nvg, cmd->lineHeight);

	return (const void *)(cmd + 1);
}

const void *RB_DrawText(const void *data) {
	auto cmd = (const drawTextCommand_t *)data;
	const char *text = (const char *)cmd + sizeof(drawTextCommand_t);

	//nvgSave(inf.nvg);
	//nvgTextBox(inf.nvg, cmd->x, cmd->y, cmd->w, text, 0);
	//nvgRestore(inf.nvg);

	return (const void *)(text + cmd->strSz);
}

const void *RB_DrawBmpText(const void *data) {
	auto cmd = (const drawBmpTextCommand_t *)data;
	const char *text = (const char *)cmd + sizeof(drawBmpTextCommand_t);

	BMPFNT_DrawText(cmd->fntId, cmd->x, cmd->y, cmd->scale, text);

	return (const void *)(text + cmd->strSz);
}

void DrawImage(float x, float y, float w, float h, float ox, float oy, float alpha, float scale, byte flipBits, Image *img, unsigned int shaderId) {
	bool flipX = flipBits & FLIP_H;
	bool flipY = flipBits & FLIP_V;
	bool flipDiag = flipBits & FLIP_DIAG;

	rlEnableTexture(img->hnd);
	rlPushMatrix();

	rlTranslatef(x, y, 0);
	rlScalef(scale, scale, 1);

	if (flipDiag) {
		rlRotatef(90, 0, 0, 1);
		rlTranslatef(0, -h, 0);
		bool tempX = flipX;
		flipX = flipY;
		flipY = !tempX;
	}

	rlBegin(RL_QUADS);
	// FIXME: alpha * 255 bad! come back to this and make it consistent everywhere?
	rlColor4ub(255, 255, 255, alpha * 255);

	rlNormal3f(0, 0, 1);

	float xTex[2] = { ox / img->w, (ox + w) / img->w };
	float yTex[2] = { oy / img->h, (oy + h) / img->h };

	// bottom left
	rlTexCoord2f(xTex[flipX ? 1 : 0], yTex[flipY ? 1 : 0]);
	rlVertex2f(0, 0);

	// bottom right
	rlTexCoord2f(xTex[flipX ? 1 : 0], yTex[flipY ? 0 : 1]);
	rlVertex2f(0, h);

	// top right
	rlTexCoord2f(xTex[flipX ? 0 : 1], yTex[flipY ? 0 : 1]);
	rlVertex2f(w, h);

	// top left
	rlTexCoord2f(xTex[flipX ? 0 : 1], yTex[flipY ? 1 : 0]);
	rlVertex2f(w, 0);
	rlEnd();

	rlPopMatrix();
	rlDisableTexture();
}

const void *RB_DrawImage(const void *data) {
	auto cmd = (const drawImageCommand_t *)data;

	Image *image = Get_Img(cmd->imgId);
	float w = cmd->w == 0 ? image->w : cmd->w;
	float h = cmd->h == 0 ? image->h : cmd->h;
	DrawImage(cmd->x, cmd->y, w, h, cmd->ox, cmd->oy, cmd->alpha, cmd->scale, cmd->flipBits, image, cmd->shaderId);

	return (const void *)(cmd + 1);
}

const void *RB_DrawSprite(const void *data) {
	auto cmd = (const drawSpriteCommand_t *)data;

	Asset *asset = Asset_Get(ASSET_SPRITE, cmd->spr);
	Sprite *spr = (Sprite*)asset->resource;
	Image *img = spr->image;

	if (cmd->id > spr->maxId) {
		Com_Printf("WARNING: draw sprite %s out of index %i > %i\n", asset->name, cmd->id, spr->maxId);
		return (const void *)(cmd + 1);
	}

	DrawImage(
		cmd->x,
		cmd->y,
		spr->spriteWidth * cmd->w,
		spr->spriteHeight * cmd->h,
		(cmd->id % spr->cols) * spr->spriteWidth,
		(cmd->id / spr->cols) * spr->spriteHeight,
		cmd->alpha,
		cmd->scale,
		cmd->flipBits,
		img,
		0
	);

	return (const void *)(cmd + 1);
}

const void *RB_DrawLine(const void *data) {
	auto cmd = (const drawLineCommand_t *)data;

	rlBegin(RL_LINES);
	rlColor4ub(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
	rlVertex2f(cmd->x1, cmd->y1);
	rlVertex2f(cmd->x2, cmd->y2);
	rlEnd();

	return (const void *)(cmd + 1);
}

const void *RB_DrawCircle(const void *data) {
	auto cmd = (const drawCircleCommand_t *)data;

	rlEnableTexture(GetTextureDefault().id);

	if (cmd->outline) {
		if (rlCheckBufferLimit(RL_LINES, 2 * 36)) {
			rlglDraw();
		}

		rlBegin(RL_LINES);
		rlColor4ub(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);

		// NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
		for (int i = 0; i < 360; i += 10)
		{
			rlVertex2f(cmd->x + sinf(DEG2RAD*i)*cmd->radius, cmd->y + cosf(DEG2RAD*i)*cmd->radius);
			rlVertex2f(cmd->x + sinf(DEG2RAD*(i + 10))*cmd->radius, cmd->y + cosf(DEG2RAD*(i + 10))*cmd->radius);
		}
		rlEnd();
	}
	else {
		if (rlCheckBufferLimit(RL_QUADS, 4 * (36 / 2))) rlglDraw();


		rlBegin(RL_QUADS);
		for (int i = 0; i < 360; i += 20)
		{
			rlColor4ub(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);

			rlVertex2f(cmd->x, cmd->y);
			rlVertex2f(cmd->x + sinf(DEG2RAD*i)*cmd->radius, cmd->y + cosf(DEG2RAD*i)*cmd->radius);
			rlVertex2f(cmd->x + sinf(DEG2RAD*(i + 10))*cmd->radius, cmd->y + cosf(DEG2RAD*(i + 10))*cmd->radius);
			rlVertex2f(cmd->x + sinf(DEG2RAD*(i + 20))*cmd->radius, cmd->y + cosf(DEG2RAD*(i + 20))*cmd->radius);
		}
		rlEnd();

	}

	rlDisableTexture();

	return (const void *)(cmd + 1);
}

const void *RB_DrawTri(const void *data) {
	auto cmd = (const drawTriCommand_t *)data;

	rlEnableTexture(GetTextureDefault().id);

	if (cmd->outline) {
		rlBegin(RL_LINES);
		rlColor4ub(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);

		rlVertex2f(cmd->x1, cmd->y1);
		rlVertex2f(cmd->x2, cmd->y2);

		rlVertex2f(cmd->x2, cmd->y2);
		rlVertex2f(cmd->x3, cmd->y3);

		rlVertex2f(cmd->x3, cmd->y3);
		rlVertex2f(cmd->x1, cmd->y1);
		rlEnd();
	}
	else {
		rlBegin(RL_QUADS);
		rlColor4ub(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);

		// FIXME: order of 2 and 3 matters. how to figure this out here and swap them?
		rlVertex2f(cmd->x1, cmd->y1);
		rlVertex2f(cmd->x2, cmd->y2);
		rlVertex2f(cmd->x2, cmd->y2);
		rlVertex2f(cmd->x3, cmd->y3);
		rlEnd();
	}

	rlDisableTexture();

	return (const void *)(cmd + 1);
}

const void *RB_DrawMapLayer(const void *data) {
	auto cmd = (const drawMapCommand_t *)data;

	tmx_map *map = (tmx_map *)Asset_Get(ASSET_TILEMAP, cmd->mapId)->resource;

	tmx_layer *layer = map->ly_head;
	unsigned int i = 0;
	while (layer != nullptr && i++ != cmd->layer) {
		layer = layer->next;
	}

	assert(layer != nullptr);

	if (layer->type == L_LAYER) {
		unsigned int cellW = cmd->cellW == 0 ? map->width : cmd->cellW;
		unsigned int cellH = cmd->cellH == 0 ? map->height : cmd->cellH;

		unsigned int endX = std::min(cmd->cellX + cellW, map->width);
		unsigned int endY = std::min(cmd->cellY + cellH, map->height);

		for (unsigned int y = cmd->cellY; y < endY; y++) {
			for (unsigned int x = cmd->cellX; x < endX; x++) {
				unsigned int raw = layer->content.gids[(y*map->width) + x];
				unsigned int gid = raw & TMX_FLIP_BITS_REMOVAL;

				if (gid == 0) {
					continue;
				}

				byte flipBits = (raw & TMX_FLIPPED_HORIZONTALLY ? FLIP_H : 0) | (raw & TMX_FLIPPED_VERTICALLY ? FLIP_V : 0) | (raw & TMX_FLIPPED_DIAGONALLY ? FLIP_DIAG : 0);

				tmx_tile *tile = map->tiles[gid];
				tmx_tileset *ts = tile->tileset;
				Asset *asset = (Asset*) tile->tileset->image->resource_image;

				DrawImage(
				//  offset + current x/y         - start tile offset         
					cmd->x + x * ts->tile_width  - (cmd->cellX * ts->tile_width),
					cmd->y + y * ts->tile_height - (cmd->cellY * ts->tile_height),
					ts->tile_width,
					ts->tile_height,
					tile->ul_x,
					tile->ul_y,
					1.0f, 1.0f, flipBits, (Image*) asset->resource, 0
				);
			}
		}
	}
	return (const void *)(cmd + 1);
}

void SubmitRenderCommands(renderCommandList_t * list) {
	const void *data = list->cmds;

	while (1) {
		switch (*(const byte *)data) {

		case RC_SET_COLOR:
			data = RB_SetColor(data);
			break;

		case RC_RESET_TRANSFORM:
			data = RB_ResetTransform(data);
			break;

		case RC_TRANSFORM:
			data = RB_Transform(data);
			break;

		case RC_ROTATE:
			data = RB_Rotate(data);
			break;

		case RC_TRANSLATE:
			data = RB_Translate(data);
			break;

		case RC_SET_SCISSOR:
			data = RB_SetScissor(data);
			break;

		case RC_DRAW_RECT:
			data = RB_DrawRect(data);
			break;

		case RC_SET_TEXT_STYLE:
			data = RB_SetTextStyle(data);
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

		case RC_DRAW_SPRITE:
			data = RB_DrawSprite(data);
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

		case RC_DRAW_MAP_LAYER:
			data = RB_DrawMapLayer(data);
			break;

		case RC_END_OF_LIST:
			return;

		default:
			Com_Error(ERR_FATAL, "Bad render command byte id %i", *(const int *)data);
			return;
		}
	}
}
