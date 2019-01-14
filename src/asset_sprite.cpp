#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console/console.h"

void* Sprite_Load(Asset &asset) {
	Sprite *spr = (Sprite*)asset.resource;

	Image *img = (Image*)Img_Load(asset);

	if (img == nullptr) {
		Com_Error(ERR_DROP, "Sprite_Load: sprite loading before image");
		return nullptr;
	}

	spr->image = img;
	spr->rows = (img->h / (spr->spriteHeight + spr->marginY));
	spr->cols = (img->w / (spr->spriteWidth + spr->marginX));
	spr->maxId = spr->rows * spr->cols - 1;

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
