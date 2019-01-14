#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console/console.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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