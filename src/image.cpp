#include <nanovg.h>
#include "assetloader.h"
#include "files.h"
#include "console/console.h"

extern ClientInfo inf;

void* Img_Load(Asset &asset) {
	void *buffer;
	auto sz = FS_ReadFile(asset.path, &buffer);

	if (sz == -1) {
		Com_Error(ERR_DROP, "Couldn't read image %s", asset.path);
		return nullptr;
	}

	Image * img = new Image();
	img->nvg = inf.nvg;
	img->hnd = nvgCreateImageMem(img->nvg, NVG_IMAGE_NEAREST, (unsigned char *)buffer, sz);
	nvgImageSize(img->nvg, img->hnd, &img->w, &img->h);

	free(buffer);

	return (void*) img;
}

void Img_Free(Asset &asset) {
	Image* img = (Image*)asset.resource;
	nvgDeleteImage(img->nvg, img->hnd);
	free(img);
}

Image* Get_Img(AssetHandle id) {
	Asset* asset = Asset_Get(ASSET_IMAGE, id);
	assert(asset != nullptr && asset->resource != nullptr);
	return (Image*) asset->resource;
}

void Sprite_Set(AssetHandle assetHandle, AssetHandle imageHandle, int width, int height, int marginX, int marginY) {
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
	spr->asset = imageHandle;

	asset->resource = (void*)spr;
}

void* Sprite_Load(Asset &asset) {
	Sprite *spr = (Sprite*)asset.resource;
	Asset *imgAsset = Asset_Get(ASSET_IMAGE, spr->asset);

	assert(imgAsset != nullptr);

	if (imgAsset->loaded == false) {
		Com_Error(ERR_DROP, "Sprite_Load: sprite loading before image");
		return nullptr;
	}

	Image *img = (Image*)imgAsset->resource;
	
	spr->imageWidth = img->w;
	spr->imageHeight = img->h;
	spr->rows = (img->h / (spr->spriteHeight + spr->marginY));
	spr->cols = (img->w / (spr->spriteWidth + spr->marginX));
	spr->maxId = spr->rows * spr->cols - 1;

	return asset.resource;
}

void Sprite_Free(Asset &asset) {
	free(asset.resource);
}