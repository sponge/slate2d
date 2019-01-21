#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console/console.h"
#include <physfs.h>

typedef struct {
	const char *name;
	int16_t x;
	int16_t y;
	int16_t w;
	int16_t h;
	int16_t framex;
	int16_t framey;
	int16_t framew;
	int16_t frameh;
	byte rotated;
} SpriteImage;

typedef struct {
	const char *name;
	int numImages;
} SpriteTexture;

typedef struct {
	int numTextures;
	int numImages;
	SpriteTexture *textures;
	SpriteImage *images;
} PackedSprite;

byte ReadByte(byte **curr) {
	byte ret = (byte)**curr;
	*curr += 1;

	return ret;
}

int16_t ReadShort(byte **curr) {
	byte lo = ReadByte(curr);
	byte hi = ReadByte(curr);
	int16_t ret = (hi << 8) | lo;

	return ret;
}

const char* ReadString(byte **curr) {
	const char *name = (const char*)*curr;
	*curr += strlen(name) + 1;

	return name;
}

void* Sprite_Load(Asset &asset) {
	Sprite *spr = (Sprite*)asset.resource;

	if (strcmp("bin", FS_FileExtension(asset.path)) == 0) {
		byte *crunch;
		FS_ReadFile(asset.path, (void**)&crunch);

		byte *curr = crunch;

		int16_t totalTextures = ReadShort(&curr);
		int16_t totalImages = ReadShort(&curr);

		SpriteTexture *textures = (SpriteTexture*) malloc(sizeof(SpriteTexture) * totalTextures);
		SpriteImage *images = (SpriteImage*)malloc(sizeof(SpriteImage) * totalImages);

		for (int tex = 0; tex < totalTextures; tex++) {

			// FIXME: this is prob unnecessary. read image directly here
			textures[tex] = {
				ReadString(&curr),
				ReadShort(&curr)
			};

			Com_Printf("texture %s has %i images\n", textures[tex].name, textures[tex].numImages);

			for (int img = 0; img < textures[tex].numImages; img++) {
				images[img] = {
					ReadString(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadByte(&curr)
				};

				Com_Printf("%s (%i): pos:(%i, %i) sz:(%i, %i)\n", images[img].name, img, images[img].x, images[img].y, images[img].w, images[img].h);
			}

		}

		asset.resource = (void*)1;
	}
	else {
		Image *img = (Image*)Img_Load(asset);

		if (img == nullptr) {
			Com_Error(ERR_DROP, "Sprite_Load: image failed to load");
			return nullptr;
		}

		spr->image = img;
		spr->rows = (img->h / (spr->spriteHeight + spr->marginY));
		spr->cols = (img->w / (spr->spriteWidth + spr->marginX));
		spr->maxId = spr->rows * spr->cols - 1;
	}

	return asset.resource;
}

void Sprite_Free(Asset &asset) {
	Sprite *spr = (Sprite*)asset.resource;
	rlDeleteTextures(spr->image->hnd);
	free(spr->image);
	free(asset.resource);
}

void Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY) {
	Asset *asset = Asset_Get(ASSET_SPRITE, assetHandle);

	if (asset == nullptr) {
		Com_Error(ERR_DROP, "Sprite_Set: asset not found");
		return;
	}
	
	if (strcmp("bin", FS_FileExtension(asset->path)) == 0) {
		Com_Error(ERR_DROP, "Sprite_Set: can't call set on a crunched sprite");
		return;
	}

	if (asset->loaded == true) {
		Com_Printf("WARNING: Asset_Set: trying to set already loaded asset\n");
		return;
	}

	auto spr = new Sprite();

	spr->spriteWidth = width;
	spr->spriteHeight = height;
	spr->marginX = marginX;
	spr->marginY = marginY;

	asset->resource = (void*)spr;
}
