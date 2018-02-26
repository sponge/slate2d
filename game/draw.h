#pragma once

void DC_SetColor(byte which, byte r, byte g, byte b, byte a);
void DC_SetTransform(bool absolute, float a, float b, float c, float d, float e, float f);
void DC_SetScissor(float x, float y, float w, float h);
void DC_ResetScissor();
void DC_DrawRect(float x, float y, float w, float h, bool outline = false);
void DC_DrawText(float x, float y, const char *text, int align);
void DC_DrawBmpText(float x, float y, float scale, const char *text, unsigned int fntId);
void DC_DrawImage(float x, float y, float w, float h, float ox, float oy, float alpha, byte flipBits, unsigned int imgId, unsigned int shaderId);
void DC_DrawLine(float x1, float y1, float x2, float y2);
void DC_DrawCircle(float x, float y, float radius, bool outline = false);
void DC_DrawTri(float x1, float y1, float x2, float y2, float x3, float y3, bool outline = false);
void DC_Submit();
void DC_Clear();

typedef struct {
	unsigned int asset;
	int maxId;
	int imageWidth, imageHeight;
	int spriteWidth, spriteHeight;
	int marginX, marginY;
	int rows, cols;
} Sprite;

const Sprite DC_CreateSprite(unsigned int asset, int width, int height, int marginX, int marginY);
void DC_DrawSprite(const Sprite sprite, int id, float x, float y, float alpha = 1.0f, byte flipBits = 0, int w = 1, int h = 1);