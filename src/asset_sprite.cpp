#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console/console.h"
#include <physfs.h>

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
	if (strcmp("bin", FS_FileExtension(asset.path)) == 0) {
		PackedSprite *spr = new PackedSprite();

		byte *crunch;
		int len = FS_ReadFile(asset.path, (void**)&crunch);

		if (len == -1) {
			Com_Error(ERR_DROP, "Sprite_Load: couldn't read file %s", asset.path);
			return nullptr;
		}

		byte *curr = crunch;

		spr->numTextures = ReadShort(&curr);
		spr->numSprites = ReadShort(&curr);

		spr->textures = new Image[spr->numTextures];
		spr->sprites = new CrunchSprite[spr->numSprites];

		for (int tex = 0; tex < spr->numTextures; tex++) {
			const char *imgPath = ReadString(&curr);
			int16_t numSprites = ReadShort(&curr);

			Image *img = (Image*)Img_LoadPath(imgPath);
			spr->textures[tex] = *img;
			free(img);

			Com_Printf("texture %s has %i images\n", imgPath, numSprites);

			for (int id = 0; id < spr->numSprites; id++) {
				const char *name = ReadString(&curr);

				spr->sprites[id] = {
					&spr->textures[tex],
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

				Com_Printf("%s (%i): pos:(%i, %i) sz:(%i, %i)\n", name, id, spr->sprites[id].x, spr->sprites[id].y, spr->sprites[id].w, spr->sprites[id].h);
			}

		}

		asset.resource = (void*)spr;
	}
	else {
		PackedSprite *spr = (PackedSprite*)asset.resource;
		Image *img = (Image*)Img_Load(asset);

		if (img == nullptr) {
			Com_Error(ERR_DROP, "Sprite_Load: image failed to load");
			return nullptr;
		}

		spr->textures = new Image[1];
		spr->textures[0] = *img;

		int rows = (img->h / (spr->staticHeight + spr->staticMarginY));
		int cols = (img->w / (spr->staticWidth + spr->staticMarginX));

		spr->numTextures = 1;
		spr->numSprites = rows * cols;
		spr->sprites = new CrunchSprite[spr->numSprites];

		for (int i = 0; i < spr->numSprites; i++) {
			spr->sprites[i].texture = &spr->textures[0];
			spr->sprites[i].x = (int16_t)((i % cols) * spr->staticWidth);
			spr->sprites[i].y = (int16_t)((i / cols) * spr->staticHeight);
			spr->sprites[i].w = (int16_t)spr->staticWidth;
			spr->sprites[i].h = (int16_t)spr->staticHeight;
		}

		free(img);

	}

	return asset.resource;
}

void Sprite_Free(Asset &asset) {
	// FIXME: free sprites
	// Sprite *spr = (Sprite*)asset.resource;
	// rlDeleteTextures(spr->image->hnd);
	// free(spr->image);
	// free(asset.resource);
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

	auto spr = new PackedSprite();

	spr->staticWidth = width;
	spr->staticHeight = height;
	spr->staticMarginX = marginX;
	spr->staticMarginY = marginY;

	asset->resource = (void*)spr;
}
