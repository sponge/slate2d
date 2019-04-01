#pragma once

#include "external/rlgl.h"
#include "shared.h"
extern "C" {
#include "external/ini.h"
}
#include <tmx.h>

AssetHandle Asset_Find(const char *name);
Asset* Asset_Get(AssetType_t type, AssetHandle id);
AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path, int flags = 0);
void Asset_Load(AssetHandle i);
void Asset_LoadAll();
void Asset_ClearAll();
void Asset_LoadINI(const char *path);

// image assets

void* Img_Load(Asset &asset);
Image* Img_LoadPath(const char *path, int flags = 0);
void Img_Free(Asset &asset);
void Img_ParseINI(Asset &asset, ini_t *ini);
Image* Get_Img(AssetHandle id);

// sprite assets

void* Sprite_Load(Asset &asset);
void Sprite_Free(Asset &asset);
void Sprite_ParseINI(Asset & asset, ini_t * ini);
void Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY);

// ttf font assets

typedef struct TTFFont{
	int hnd;
	bool valid;
} TTFFont_t;

void* TTF_Load(Asset &asset);
void TTF_Free(Asset &asset);

void TTF_TextBox(const drawTextCommand_t *data, const char *text, int count);
const char * TTF_BreakString(int w, const char *in);
int Asset_TextWidth(AssetHandle assetHandle, const char *string, float scale);

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
void BMPFNT_ParseINI(Asset & asset, ini_t * ini);
void BMPFNT_Set(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);

// audio assets

void* Speech_Load(Asset &asset);
void Speech_Free(Asset &asset);
void Speech_ParseINI(Asset &asset, ini_t *ini);
void* Sound_Load(Asset &asset);
void Sound_Free(Asset &asset);
void Mod_Free(Asset &asset);

unsigned int Snd_Play(AssetHandle assetHandle, float volume, float pan, bool loop);
void Snd_Stop(unsigned int handle);
void Snd_PauseResume(unsigned int handle, bool pause);

// tilemap assets

void * TileMap_Load(Asset &asset);
void TileMap_Free(Asset &asset);
tmx_map* Get_TileMap(AssetHandle id);

// canvas assets

struct Canvas {
	int w, h, flags;
	RenderTexture2D texture;
};

void * Canvas_Load(Asset &asset);
void Canvas_Set(AssetHandle id, int width, int height);
void Canvas_Free(Asset &asset);
void Canvas_ParseINI(Asset & asset, ini_t * ini);

// shader assets

struct ShaderAsset {
	char *vs, *fs;
	bool isFile;
	int locResolution, locTime, locTimeDelta, locMouse;
	Shader *shader;
};

void * Shader_Load(Asset & asset);
void Shader_Set(AssetHandle id, bool isFile, const char *vs, const char *fs);
void Shader_Free(Asset & asset);
void Shader_ParseINI(Asset & asset, ini_t * ini);
