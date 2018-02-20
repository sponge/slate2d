#include <nanovg.h>
#include "assetloader.h"
#include "files.h"
#include "console/console.h"

extern ClientInfo inf;

void* Img_Load(Asset &asset) {
	void *buffer;
	auto sz = FS_ReadFile(asset.path, &buffer);

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
	Asset* asset = Asset_Get(id);
	if (asset->type != ASSET_IMAGE) {
		return nullptr;
	}

	return (Image*) asset->resource;
}