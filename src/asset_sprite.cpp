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
	// if the sprite ends in bin, load it through crunch, otherwise generate the sprite
	if (strcmp("bin", FS_FileExtension(asset.path)) == 0) {
		// read the bin file
		byte *crunch;
		int len = FS_ReadFile(asset.path, (void**)&crunch);
		byte *curr = crunch;

		if (len == -1) {
			Com_Error(ERR_DROP, "Sprite_Load: couldn't read file %s", asset.path);
			return nullptr;
		}

		// we have a probably valid file, create the atlas
		SpriteAtlas *atlas = new SpriteAtlas();

		// read the counts from the file (numSprites is a new field compared to standard crunch)
		atlas->numTextures = ReadShort(&curr);
		atlas->numSprites = ReadShort(&curr);

		atlas->textures = new Image[atlas->numTextures];
		atlas->sprites = new Sprite[atlas->numSprites];

		// for each texture, read the texture path and number of sprites in that image
		for (int tex = 0; tex < atlas->numTextures; tex++) {
			const char *imgPath = ReadString(&curr);
			int16_t texSprites = ReadShort(&curr);

			// load the imgae into the GPU, copy it, and delete it
			Image *img = (Image*)Img_LoadPath(imgPath);
			atlas->textures[tex] = *img;
			delete img;

			Com_Printf("texture %s has %i images\n", imgPath, texSprites);

			// for each sprite in the texture, read it into our struct
			for (int i = 0; i <= texSprites; i++) {
				// we don't care about the name, just print it
				const char *name = ReadString(&curr);

				// read all the sprite attributes
				atlas->sprites[i] = {
					&atlas->textures[tex],
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

				Sprite *spr = &atlas->sprites[i];
				Com_Printf("%s (%i): pos:(%i, %i) sz:(%i, %i)\n", name, i, spr->x, spr->y, spr->w, spr->h);
			}

		}

		asset.resource = (void*)atlas;
	}
	else {
		// create the atlas based off of fixed bounds
		SpriteAtlas *spr = (SpriteAtlas*)asset.resource;
		Image *img = (Image*)Img_Load(asset);

		if (img == nullptr) {
			Com_Error(ERR_DROP, "Sprite_Load: image failed to load");
			return nullptr;
		}

		// sprites set with setSprite only ever have one texture
		spr->textures = new Image[1];
		spr->textures[0] = *img;

		int rows = (img->h / (spr->staticHeight + spr->staticMarginY));
		int cols = (img->w / (spr->staticWidth + spr->staticMarginX));

		delete img;

		spr->numTextures = 1;
		spr->numSprites = rows * cols;
		spr->sprites = new Sprite[spr->numSprites];

		// step through each square in the grid and generate the structure for it
		for (int i = 0; i < spr->numSprites; i++) {
			spr->sprites[i].texture = &spr->textures[0];
			spr->sprites[i].x = (int16_t)((i % cols) * spr->staticWidth);
			spr->sprites[i].y = (int16_t)((i / cols) * spr->staticHeight);
			spr->sprites[i].w = (int16_t)spr->staticWidth;
			spr->sprites[i].h = (int16_t)spr->staticHeight;
		}

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

	auto spr = new SpriteAtlas();

	spr->staticWidth = width;
	spr->staticHeight = height;
	spr->staticMarginX = marginX;
	spr->staticMarginY = marginY;

	asset->resource = (void*)spr;
}
