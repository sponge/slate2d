#pragma once
#include "../game/shared.h"

void SubmitRenderCommands(renderCommandList_t *list);
void DrawImage(float x, float y, float w, float h, float ox, float oy, float alpha, float scale, byte flipBits, unsigned int handle, int imgW, int imgH);

struct RenderState {
	byte color[4] = { 255, 255, 255, 255 };
	Asset *font;
	int align = 1;
	float lineHeight = 1.0f;
	float size = 1.0f;
};
typedef struct RenderState RenderState;

extern RenderState state;