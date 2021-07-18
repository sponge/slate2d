#include <assert.h>
#include "rendercommands.h"
#include "assetloader.h"
#include "rlgl.h"
#include "main.h"
#include "input.h"
#include "external/fontstash.h"
#include "console.h"

extern conVar_t* vid_width, * vid_height;
Canvas * activeCanvas = nullptr;
RenderState state;

const void *RB_SetColor(const void *data) {
	auto cmd = (const setColorCommand_t *)data;

	state.color[0] = cmd->color[0];
	state.color[1] = cmd->color[1];
	state.color[2] = cmd->color[2];
	state.color[3] = cmd->color[3];

	return (const void *)(cmd + 1);
}

const void *RB_Clear(const void *data) {
	auto cmd = (const clearCommand_t *)data;

	rlClearColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);
	rlClearScreenBuffers();

	return (const void *)(cmd + 1);
}

const void *RB_ResetTransform(const void *data) {
	auto cmd = (const resetTransformCommand_t *)data;

	rlLoadIdentity();

	return (const void *)(cmd + 1);
}

const void *RB_Scale(const void *data) {
	auto cmd = (const scaleCommand_t *)data;

	rlScalef(cmd->x, cmd->y, 1.0f);

	return (const void *)(cmd + 1);
}

const void *RB_Rotate(const void *data) {
	auto cmd = (const rotateCommand_t *)data;

	rlRotatef(cmd->angle, 0, 0, 1);

	return (const void *)(cmd + 1);
}

const void *RB_Translate(const void *data) {
	auto cmd = (const translateCommand_t *)data;

	rlTranslatef(cmd->x, cmd->y, 0);

	return (const void *)(cmd + 1);
}

const void *RB_SetScissor(const void *data) {
	auto cmd = (const setScissorCommand_t *)data;

	if (cmd->w <= 0 || cmd->h <= 0) {
		rlDisableScissorTest();
	}
	else {
		rlEnableScissorTest();
		rlScissor(cmd->x, cmd->y, cmd->w, cmd->h);
	}

	return (const void *)(cmd + 1);
}

const void *RB_UseCanvas(const void *data) {
	auto cmd = (const useCanvasCommand_t*)data;

	Canvas *canvas = (Canvas *)Asset_Get(ASSET_CANVAS, cmd->canvasId)->resource;

	assert(canvas != nullptr);

	rlEnableFramebuffer(canvas->id);

	// Initialize viewport and internal projection/modelview matrices
	rlViewport(0, 0, canvas->w, canvas->h);
	rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
	rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
	rlOrtho(0, canvas->w, canvas->h, 0, 0.0f, 1.0f); // Orthographic projection with top-left corner at (0,0)
	rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
	rlLoadIdentity();                                   // Reset current matrix (MODELVIEW)

	activeCanvas = canvas;

	return (const void *)(cmd + 1);
}

const void *RB_ResetCanvas(const void *data) {
	auto cmd = (const resetCanvasCommand_t*)data;

	rlDisableFramebuffer();
	// Initialize viewport and internal projection/modelview matrices
	rlViewport(0, 0, vid_width->integer, vid_height->integer);
	rlMatrixMode(RL_PROJECTION);                        // Switch to PROJECTION matrix
	rlLoadIdentity();                                   // Reset current matrix (PROJECTION)
	rlOrtho(0, vid_width->integer, vid_height->integer, 0, 0.0f, 1.0f); // Orthographic projection with top-left corner at (0,0)
	rlMatrixMode(RL_MODELVIEW);                         // Switch back to MODELVIEW matrix
	rlLoadIdentity();

	activeCanvas = nullptr;

	return (const void *)(cmd + 1);
}

const void *RB_UseShader(const void *data) {
	auto cmd = (const useShaderCommand_t*)data;

	ShaderAsset *shasset = (ShaderAsset *)Asset_Get(ASSET_SHADER, cmd->shaderId)->resource;

	assert(shasset != nullptr);

	rlEnableShader(shasset->id);

	if (shasset->locResolution != -1) {
		if (activeCanvas != nullptr) {
			const float iResolution[3] = { (float) activeCanvas->w, (float) activeCanvas->h, 1.0f };
			rlSetUniform(shasset->locResolution, iResolution, SHADER_UNIFORM_VEC3, 1);
		}
		else {
			const float iResolution[3] = { vid_width->value, vid_height->value, 1.0f };
			rlSetUniform(shasset->locResolution, iResolution, SHADER_UNIFORM_VEC3, 1);
		}
	}

	if (shasset->locTime != -1) {
		const float iTime = com_frameTime / (float)1E6;
        rlSetUniform(shasset->locTime, &iTime, SHADER_UNIFORM_FLOAT, 1);
	}

	if (shasset->locTimeDelta != -1) {
		const float iTimeDelta = frame_musec / (float)1E6;
        rlSetUniform(shasset->locTimeDelta, &iTimeDelta, SHADER_UNIFORM_FLOAT, 1);
	}

	if (shasset->locMouse != -1) {
		auto mousePos = In_MousePosition();
		const float iMouse[2] = { (float) mousePos.x, (float) mousePos.y };
		rlSetUniform(shasset->locMouse, iMouse, SHADER_UNIFORM_VEC2, 1);
	}

	return (const void *)(cmd + 1);
}

const void *RB_ResetShader(const void *data) {
	auto cmd = (const resetShaderCommand_t*)data;

	rlDisableShader();

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
    rlSetTexture(rlGetTextureDefault().id);
	rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);

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

	Asset *asset = Asset_Get(ASSET_ANY, cmd->fntId);

	assert(asset != nullptr);

	if (asset->type != ASSET_BITMAPFONT && asset->type != ASSET_FONT) {
		Con_Error(ERR_GAME, "asset not font or bmpfont");
		return (const void *)(cmd + 1);
	}

	state.lineHeight = cmd->lineHeight;
	state.align = cmd->align;

	if (asset->type == ASSET_FONT) {
		TTFFont_t *fnt = (TTFFont_t*)asset->resource;
		fonsSetSpacing(ctx, 0);
		fonsSetFont(ctx, fnt->hnd);
	} else {
		BitmapFont_t *fnt = (BitmapFont_t*)asset->resource;
		fonsSetSpacing(ctx, fnt->charSpacing);
		fonsSetFont(ctx, fnt->hnd);
	}

	fonsSetSize(ctx, (float)cmd->size);
	fonsSetAlign(ctx, cmd->align);

	return (const void *)(cmd + 1);
}

const void *RB_DrawText(const void *data) {
	auto cmd = (const drawTextCommand_t *)data;
	const char *text = (const char *)cmd + sizeof(drawTextCommand_t);

	fonsSetColor(ctx, (state.color[0]) | (state.color[1] << 8) | (state.color[2] << 16) | (state.color[3] << 24));
	TTF_TextBox(cmd->x, cmd->y, cmd->w, text, cmd->len);

	return (const void *)(text + cmd->strSz);
}

void DrawImage(float x, float y, float w, float h, float ox, float oy, float scale, uint8_t flipBits, unsigned int handle, int imgW, int imgH) {
	bool flipX = flipBits & FLIP_H;
	bool flipY = flipBits & FLIP_V;
	bool flipDiag = flipBits & FLIP_DIAG;

	rlSetTexture(handle);
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
	rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);

	rlNormal3f(0, 0, 1);

	float xTex[2] = { ox / imgW, (ox + w) / imgW };
	float yTex[2] = { oy / imgH, (oy + h) / imgH };

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

	auto *asset = Asset_Get(ASSET_ANY, cmd->imgId);

	assert(asset != nullptr && asset->resource != nullptr);

	if (asset->type == ASSET_CANVAS) {
		Canvas *canvas = (Canvas*) asset->resource;
		float w = cmd->w == 0 ? canvas->w : cmd->w;
		float h = cmd->h == 0 ? canvas->h : cmd->h;
		uint8_t flipBits = cmd->flipBits;
		flipBits ^= FLIP_V;
		DrawImage(cmd->x, cmd->y, w, h, cmd->ox, cmd->oy, cmd->scale, flipBits, canvas->texId, canvas->w, canvas->h);
	}
	else {
		Image *image = Get_Img(cmd->imgId);
		float w = cmd->w == 0 ? image->w : cmd->w;
		float h = cmd->h == 0 ? image->h : cmd->h;
		DrawImage(cmd->x, cmd->y, w, h, cmd->ox, cmd->oy, cmd->scale, cmd->flipBits, image->hnd, image->w, image->h);
	}
	return (const void *)(cmd + 1);
}

const void *RB_DrawSprite(const void *data) {
	auto cmd = (const drawSpriteCommand_t *)data;

	Asset *asset = Asset_Get(ASSET_SPRITE, cmd->spr);
	SpriteAtlas *spr = (SpriteAtlas*)asset->resource;

	if (cmd->id > spr->numSprites) {
		Con_Printf("WARNING: draw sprite %s out of index %i > %i\n", asset->name, cmd->id, spr->numSprites - 1);
		return (const void *)(cmd + 1);
	}

	Sprite *crunch = &spr->sprites[cmd->id];
	Image *img = crunch->texture;

	DrawImage(
		cmd->x - crunch->framex,
		cmd->y - crunch->framey,
		(float)crunch->w * cmd->w,
		(float)crunch->h * cmd->h,
		(float)crunch->x,
		(float)crunch->y,
		cmd->scale,
		cmd->flipBits,
		img->hnd,
		img->w,
		img->h
	);

	return (const void *)(cmd + 1);
}

const void *RB_DrawLine(const void *data) {
	auto cmd = (const drawLineCommand_t *)data;

	rlSetTexture(rlGetTextureDefault().id);
	rlBegin(RL_LINES);
	rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);
	rlVertex2f(cmd->x1, cmd->y1);
	rlVertex2f(cmd->x2, cmd->y2);
	rlEnd();

	return (const void *)(cmd + 1);
}

const void *RB_DrawCircle(const void *data) {
	auto cmd = (const drawCircleCommand_t *)data;

	rlSetTexture(rlGetTextureDefault().id);

	if (cmd->outline) {
		if (rlCheckRenderBatchLimit(2 * 36)) {
			rlDrawRenderBatchActive();
		}

		rlBegin(RL_LINES);
		rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);

		// NOTE: Circle outline is drawn pixel by pixel every degree (0 to 360)
		for (int i = 0; i < 360; i += 10)
		{
			rlVertex2f(cmd->x + sinf((float)DEG2RAD*i)*cmd->radius, cmd->y + cosf((float)DEG2RAD*i)*cmd->radius);
			rlVertex2f(cmd->x + sinf((float)DEG2RAD*(i + 10))*cmd->radius, cmd->y + cosf((float)DEG2RAD*(i + 10))*cmd->radius);
		}
		rlEnd();
	}
	else {
		if (rlCheckRenderBatchLimit(4 * (36 / 2))) {
			rlDrawRenderBatchActive();
		}

		rlBegin(RL_QUADS);
		for (int i = 0; i < 360; i += 20)
		{
			rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);

			rlVertex2f(cmd->x, cmd->y);
			rlVertex2f(cmd->x + sinf((float)DEG2RAD*i)*cmd->radius, cmd->y + cosf((float)DEG2RAD*i)*cmd->radius);
			rlVertex2f(cmd->x + sinf((float)DEG2RAD*(i + 10))*cmd->radius, cmd->y + cosf((float)DEG2RAD*(i + 10))*cmd->radius);
			rlVertex2f(cmd->x + sinf((float)DEG2RAD*(i + 20))*cmd->radius, cmd->y + cosf((float)DEG2RAD*(i + 20))*cmd->radius);
		}
		rlEnd();

	}

	rlDisableTexture();

	return (const void *)(cmd + 1);
}

const void *RB_DrawTri(const void *data) {
	auto cmd = (const drawTriCommand_t *)data;

	rlSetTexture(rlGetTextureDefault().id);

	if (cmd->outline) {
		rlBegin(RL_LINES);
		rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);

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
		rlColor4ub(state.color[0], state.color[1], state.color[2], state.color[3]);

		rlVertex2f(cmd->x1, cmd->y1);
		rlVertex2f(cmd->x2, cmd->y2);
		rlVertex2f(cmd->x2, cmd->y2);
		rlVertex2f(cmd->x3, cmd->y3);
		rlEnd();
	}

	rlDisableTexture();

	return (const void *)(cmd + 1);
}

const void *RB_DrawTilemap(const void *data) {
	auto cmd = (const drawMapCommand_t *)data;
	const int *tiles = (const int *)(cmd + 1);

	Asset* asset = Asset_Get(ASSET_SPRITE, cmd->sprId);
	SpriteAtlas* spr = (SpriteAtlas*)asset->resource;

	for (int i = 0; i < cmd->w * cmd->h; i++) {
		if (tiles[i] < 0)
			continue;

		int x = i % cmd->w;
		int y = i / cmd->w;
		Sprite* crunch = &spr->sprites[tiles[i]];
		Image* img = crunch->texture;
		DrawImage(
			//  offset + current x/y         - start tile offset         
			cmd->x + x * crunch->w,
			cmd->y + y * crunch->h,
			(float)crunch->w,
			(float)crunch->h,
			(float)crunch->x,
			(float)crunch->y,
			1.0f,
			0,
			img->hnd,
			img->w,
			img->h
		);
	}

	return (const void*)(tiles + cmd->w * cmd->h);
}

void SubmitRenderCommands(renderCommandList_t * list) {
	const void *data = list->cmds;

	while (1) {
		switch (*(const uint8_t *)data) {

		case RC_SET_COLOR:
			data = RB_SetColor(data);
			break;

		case RC_CLEAR:
			data = RB_Clear(data);
			break;

		case RC_RESET_TRANSFORM:
			rlDrawRenderBatchActive();
			data = RB_ResetTransform(data);
			break;

		case RC_SCALE:
			rlDrawRenderBatchActive();
			data = RB_Scale(data);
			break;

		case RC_ROTATE:
			rlDrawRenderBatchActive();
			data = RB_Rotate(data);
			break;

		case RC_TRANSLATE:
			rlDrawRenderBatchActive();
			data = RB_Translate(data);
			break;

		case RC_SET_SCISSOR:
			rlDrawRenderBatchActive();
			data = RB_SetScissor(data);
			break;

		case RC_USE_CANVAS:
			rlDrawRenderBatchActive();
			data = RB_UseCanvas(data);
			break;

		case RC_RESET_CANVAS:
			rlDrawRenderBatchActive();
			data = RB_ResetCanvas(data);
			break;

		case RC_USE_SHADER:
			rlDrawRenderBatchActive();
			data = RB_UseShader(data);
			break;

		case RC_RESET_SHADER:
			rlDrawRenderBatchActive();
			data = RB_ResetShader(data);
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

		case RC_DRAW_TILEMAP:
			data = RB_DrawTilemap(data);
			break;

		case RC_END_OF_LIST:
			rlDrawRenderBatchActive();
			return;

		default:
			Con_Errorf(ERR_FATAL, "Bad render command byte id %i", *(const int *)data);
			return;
		}
	}
}
