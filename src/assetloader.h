#pragma once

#include "../game/shared.h"

AssetHandle Asset_Find(const char *name);
Asset* Asset_Get(AssetHandle id);
AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path);
void Asset_LoadAll();
void Asset_ClearAll();

// image assets

typedef struct {
	struct NVGcontext *nvg;
	unsigned int hnd;
	int w, h;
} Image;

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
	unsigned int index;
	unsigned int hnd;
	int charSpacing, spaceWidth, lineHeight;
	int w, h;
	unsigned char glyphs[256];
	BitmapGlyph offsets[256];
	char name[64];
	char path[64];
} BitmapFont_t;

// audio assets

void* Speech_Load(Asset &asset);
void Speech_Free(Asset &asset);
void* Sound_Load(Asset &asset);
void Sound_Free(Asset &asset);
void Mod_Free(Asset &asset);