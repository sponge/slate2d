#pragma once

void DC_SetColor(byte which, byte r, byte g, byte b, byte a);
void DC_ResetTransform();
void DC_Transform(float a, float b, float c, float d, float e, float f);
void DC_Rotate(float angle);
void DC_Translate(float x, float y);
void DC_SetScissor(float x, float y, float w, float h);
void DC_ResetScissor();
void DC_DrawRect(float x, float y, float w, float h, bool outline = false);
void DC_SetTextStyle(unsigned int fntId, unsigned int size, float lineHeight);
void DC_DrawText(float x, float y, float h, const char *text);
void DC_DrawBmpText(unsigned int fntId, float x, float y, const char *text, float scale = 1.0f);
void DC_DrawImage(unsigned int imgId, float x, float y, float w = 0.0f, float h = 0.0f, float alpha = 1.0f, float scale = 1.0f, byte flipBits = 0, float ox = 0.0f, float oy = 0.0f, unsigned int shaderId = 0);
void DC_DrawSprite(unsigned int sprite, int id, float x, float y, float alpha = 1.0f, float scale = 1.0f, byte flipBits = 0, int w = 1, int h = 1);
void DC_DrawLine(float x1, float y1, float x2, float y2);
void DC_DrawCircle(float x, float y, float radius, bool outline = false);
void DC_DrawTri(float x1, float y1, float x2, float y2, float x3, float y3, bool outline = false);
void DC_DrawMapLayer(unsigned int layer, float x = 0, float y = 0, unsigned int cellX = 0, unsigned int cellY = 0, unsigned int cellW = 0, unsigned int cellH = 0);
void DC_Submit();
void DC_Clear();