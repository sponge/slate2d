#pragma once

typedef int AssetHandle;

#define INVALID_ASSET -1

typedef enum {
	ASSET_IMAGE,
	/*
	ASSET_FONT,
	ASSET_BITMAPFONT,
	ASSET_SOUND,
	*/
	ASSET_MAX
} AssetType_t;

typedef struct {
	AssetHandle id;
	AssetType_t type;
	char name[64];
	char path[64];
	void *resource;
} Asset;

AssetHandle Asset_Find(const char *name);
Asset* Asset_Get(AssetHandle id);
AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path);
void Asset_LoadAll();
void Asset_ClearAll();

typedef struct {
	struct NVGcontext *nvg;
	unsigned int hnd;
	int w, h;
} Image;

void* Img_Load(Asset &asset);
void Img_Free(Asset &asset);
Image* Get_Img(AssetHandle id);

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