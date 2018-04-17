#pragma once

#include "../game/shared.h"

AssetHandle Asset_Find(const char *name);
Asset* Asset_Get(AssetType_t type, AssetHandle id);
AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path);
void Asset_LoadAll();
void Asset_ClearAll();

// image assets

void* Img_Load(Asset &asset);
void Img_Free(Asset &asset);
Image* Get_Img(AssetHandle id);

// bitmap font assets

typedef struct {
	char glyph;
	int start;
	int end;
} BitmapGlyph;

typedef struct BitmapFont {
	struct NVGcontext *nvg;
	unsigned int hnd;
	int charSpacing, glyphWidth, spaceWidth, lineHeight;
	int w, h;
	unsigned char glyphs[256];
	BitmapGlyph offsets[256];
} BitmapFont_t;

void* BMPFNT_Load(Asset &asset);
void BMPFNT_Free(Asset &asset);
void BMPFNT_Set(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);

// audio assets

void* Speech_Load(Asset &asset);
void Speech_Free(Asset &asset);
void* Sound_Load(Asset &asset);
void Sound_Free(Asset &asset);
void Mod_Free(Asset &asset);