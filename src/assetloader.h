#pragma once

#include "external/rlgl.h"
#include "slate2d.h"
extern "C" {
#include "external/ini.h"
}

typedef struct {
	AssetHandle id;
	AssetType_t type;
	bool loaded;
	const char* name;
	const char* path;
	int flags;
	void* resource;
} Asset;

AssetHandle Asset_Find(const char *name);
Asset* Asset_Get(AssetType_t type, AssetHandle id);
AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path, int flags = 0);
void Asset_Load(AssetHandle i);
void Asset_Unload(AssetHandle i);
void Asset_LoadAll();
void Asset_ClearAll();
void Asset_LoadINI(const char *path);
void Asset_DrawInspector();

// image assets

void* Img_Load(Asset &asset);
Image* Img_LoadPath(const char *path, int flags = 0);
void Img_Free(Asset &asset);
void Img_Reload(Asset &asset);
void Img_ParseINI(Asset &asset, ini_t *ini);
const Image* Get_Img(AssetHandle id);
void Img_Inspect(Asset& asset, bool deselected);

// sprite assets

typedef struct {
	Image* texture;
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
	int16_t framex;
	int16_t framey;
	int16_t framew;
	int16_t frameh;
	uint8_t rotated;
} Sprite;

typedef struct {
	int numImages;
	int numSprites;
	Image* images;
	Sprite* sprites;

	int spriteWidth;
	int spriteHeight;
	int spriteMarginX;
	int spriteMarginY;
} SpriteAtlas;

void* Sprite_Load(Asset &asset);
void Sprite_Free(Asset &asset);
void Sprite_Reload(Asset& asset);
void Sprite_ParseINI(Asset & asset, ini_t * ini);
void Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY);
void Sprite_Inspect(Asset& asset, bool deselected);

// ttf font assets

typedef struct TTFFont{
	int hnd;
	bool valid;
} TTFFont_t;

void* TTF_Load(Asset &asset);
void TTF_Free(Asset &asset);

void TTF_TextBox(float x, float y, float w, const char *text, int count);
const char * TTF_BreakString(int w, const char *in);
Dimensions Asset_TextSize(float w, const char *string, int count);

// bitmap font assets

typedef struct {
	char glyph;
	int start;
	int end;
} BitmapGlyph;

typedef struct BitmapFont {
	unsigned char *img;
	int charSpacing, glyphWidth, spaceWidth, lineHeight;
	int hnd, w, h;
	unsigned char glyphs[256];
	BitmapGlyph offsets[256];
} BitmapFont_t;

void* BMPFNT_Load(Asset &asset);
void BMPFNT_Free(Asset &asset);
void BMPFNT_Reload(Asset &asset);
void BMPFNT_ParseINI(Asset & asset, ini_t * ini);
void BMPFNT_Set(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);
void BMPFNT_Inspect(Asset& asset, bool deselected);

// audio assets

void* Speech_Load(Asset &asset);
void Speech_Free(Asset &asset);
void Speech_ParseINI(Asset &asset, ini_t *ini);
void* Sound_Load(Asset &asset);
void Sound_Free(Asset &asset);
void Mod_Free(Asset &asset);
void Sound_Inspect(Asset& asset, bool deselected);


unsigned int Snd_Play(AssetHandle assetHandle, float volume, float pan, bool loop);
void Snd_Stop(unsigned int handle);
void Snd_PauseResume(unsigned int handle, bool pause);

// canvas assets

struct Canvas {
	int w, h, flags;
	unsigned int id, texId;
};

void * Canvas_Load(Asset &asset);
void Canvas_Set(AssetHandle id, int width, int height);
void Canvas_Free(Asset &asset);
void Canvas_ParseINI(Asset & asset, ini_t * ini);
void Canvas_Inspect(Asset& asset, bool deselected);

// shader assets

struct ShaderAsset {
	char *vs, *fs;
	bool isFile;
	int locResolution, locTime, locTimeDelta, locMouse;
	unsigned int id;
};

void * Shader_Load(Asset & asset);
void Shader_Set(AssetHandle id, bool isFile, const char *vs, const char *fs);
void Shader_Free(Asset & asset);
void Shader_ParseINI(Asset & asset, ini_t * ini);
void Shader_Inspect(Asset& asset, bool deselected);
