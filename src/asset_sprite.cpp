#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console.h"
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
			Con_Error(ERR_GAME, "Sprite_Load: couldn't read file %s", asset.path);
			return nullptr;
		}

		// we have a probably valid file, create the atlas
		SpriteAtlas *atlas = new SpriteAtlas();

		// read the counts from the file (numSprites is a new field compared to standard crunch)
		atlas->numImages = ReadShort(&curr);
		atlas->numSprites = ReadShort(&curr);

		atlas->images = new Image[atlas->numImages];
		atlas->sprites = new Sprite[atlas->numSprites];

		// for each texture, read the texture path and number of sprites in that image
		for (int tex = 0; tex < atlas->numImages; tex++) {
			const char *imgPath = ReadString(&curr);
			int16_t texSprites = ReadShort(&curr);

			// load the imgae into the GPU, copy it, and delete it
			Image *img = Img_LoadPath(imgPath, asset.flags);
			atlas->images[tex] = *img;
			delete img;

			Con_Printf("texture %s has %i images\n", imgPath, texSprites);

			// for each sprite in the texture, read it into our struct
			for (int i = 0; i < texSprites; i++) {
				// we don't care about the name, just print it
				const char *name = ReadString(&curr);

				// read all the sprite attributes
				atlas->sprites[i] = {
					&atlas->images[tex],
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
				Con_Printf("%s (%i): pos:(%i, %i) sz:(%i, %i)\n", name, i, spr->x, spr->y, spr->w, spr->h);
			}

		}

		free(crunch);
		asset.resource = (void*)atlas;
	}
	else {
		// create the atlas based off of fixed bounds
		SpriteAtlas *spr = (SpriteAtlas*)asset.resource;

		if (spr->staticWidth == 0 || spr->staticHeight == 0) {
			Con_Error(ERR_GAME, "Sprite_Load: staticWidth and staticHeight must not be 0 (missing Sprite_Set?)");
		}

		Image *img = (Image*)Img_Load(asset);

		if (img == nullptr) {
			Con_Error(ERR_GAME, "Sprite_Load: image failed to load");
			return nullptr;
		}

		// sprites set with setSprite only ever have one texture
		spr->images = new Image[1];
		spr->images[0] = *img;

		int rows = (img->h / (spr->staticHeight + spr->staticMarginY));
		int cols = (img->w / (spr->staticWidth + spr->staticMarginX));

		delete img;

		spr->numImages = 1;
		spr->numSprites = rows * cols;
		spr->sprites = new Sprite[spr->numSprites];

		// step through each square in the grid and generate the structure for it
		for (int i = 0; i < spr->numSprites; i++) {
			spr->sprites[i] = { 0 };
			spr->sprites[i].texture = &spr->images[0];
			spr->sprites[i].x = (int16_t)((i % cols) * spr->staticWidth);
			spr->sprites[i].y = (int16_t)((i / cols) * spr->staticHeight);
			spr->sprites[i].w = (int16_t)spr->staticWidth;
			spr->sprites[i].h = (int16_t)spr->staticHeight;
		}

	}

	return asset.resource;
}

void Sprite_Free(Asset &asset) {
	SpriteAtlas *spr = (SpriteAtlas*)asset.resource;
	delete[] spr->sprites;

	for (int i = 0; i < spr->numImages; i++) {
		rlDeleteTextures(spr->images[i].hnd);
	}

	delete[] spr->images;

	delete asset.resource;
}

void Sprite_ParseINI(Asset &asset, ini_t *ini) {
	int width = 0, height = 0, marginX = 0, marginY = 0;
	ini_sget(ini, asset.name, "width", "%i", &width);
	ini_sget(ini, asset.name, "height", "%i", &height);
	ini_sget(ini, asset.name, "marginx", "%i", &marginX);
	ini_sget(ini, asset.name, "marginy", "%i", &marginY);

	Sprite_Set(asset.id, width, height, marginX, marginY);
}

void Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY) {
	Asset *asset = Asset_Get(ASSET_SPRITE, assetHandle);

	if (asset == nullptr) {
		Con_Error(ERR_GAME, "Sprite_Set: asset not found");
		return;
	}
	
	if (strcmp("bin", FS_FileExtension(asset->path)) == 0) {
		Con_Error(ERR_GAME, "Sprite_Set: can't call set on a crunched sprite");
		return;
	}

	if (width <= 0 || height <= 0) {
		Con_Error(ERR_GAME, "Sprite_Set: width and height must be greater than 0");
		return;
	}

	if (asset->loaded == true) {
		Con_Printf("WARNING: Asset_Set: trying to set already loaded asset\n");
		return;
	}

	auto spr = new SpriteAtlas();

	spr->staticWidth = width;
	spr->staticHeight = height;
	spr->staticMarginX = marginX;
	spr->staticMarginY = marginY;

	asset->resource = (void*)spr;
}
