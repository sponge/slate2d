#pragma once
#include <nanovg.h>
#include "../game/shared.h"

BitmapFont * BMPFNT_Create(const char *name, const char *path, const char *glyphs, int charSpacing, int spaceWidth, int lineHeight);
void BMPFNT_Load(BitmapFont &font);
BitmapFont * BMPFNT_Get(unsigned int i);
BitmapFont * BMPFNT_Find(const char *name);
void BMPFNT_LoadAll();
bool BMPFNT_Free(const char *name);
void BMPFNT_Clear();
int BMPFNT_TextWidth(BitmapFont &font, const char *string);
int BMPFNT_DrawText(BitmapFont &font, float x, float y, float scale, const char *string);