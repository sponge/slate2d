#pragma once

void DC_SetColor(byte r, byte g, byte b, byte a);
void DC_SetColor(byte color[4]);
void DC_SetTransform(float a, float b, float c, float d, float e, float f);
void DC_DrawRect(float x, float y, float w, float h);
void DC_DrawText(float x, float y, const char *text, int align);
void DC_Submit();
void DC_Clear();