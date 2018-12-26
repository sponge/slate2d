#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console/console.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

extern ClientInfo inf;

void* Img_Load(Asset &asset) {
	unsigned char *buffer;
	auto sz = FS_ReadFile(asset.path, (void**) &buffer);

	if (sz == -1) {
		Com_Error(ERR_DROP, "Couldn't read image %s", asset.path);
		return nullptr;
	}

	Image * img = new Image();

	int imgBpp;
	unsigned char *loaded = stbi_load_from_memory(buffer, sz, &img->w, &img->h, &imgBpp, 0);

	// FIXME: error handling?

	unsigned int format = 0;
	if (imgBpp == 1) format = UNCOMPRESSED_GRAYSCALE;
	else if (imgBpp == 2) format = UNCOMPRESSED_GRAY_ALPHA;
	else if (imgBpp == 3) format = UNCOMPRESSED_R8G8B8;
	else if (imgBpp == 4) format = UNCOMPRESSED_R8G8B8A8;

	unsigned int tex = rlLoadTexture(loaded, img->w, img->h, format, 1);

	// FIXME: error handling?

	// FIXME: nearest flags

	img->hnd = tex;

	free(buffer);

	return (void*) img;
}

void Img_Free(Asset &asset) {
	Image* img = (Image*)asset.resource;

	rlDeleteTextures(img->hnd);
	free(img);
}

Image* Get_Img(AssetHandle id) {
	Asset* asset = Asset_Get(ASSET_IMAGE, id);
	assert(asset != nullptr && asset->resource != nullptr);
	return (Image*) asset->resource;
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
	//nvgDeleteImage(spr->image->nvg, spr->image->h);
	free(spr->image);
	free(asset.resource);
}