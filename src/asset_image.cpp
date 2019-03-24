#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image* Img_LoadPath(const char *path, int flags) {
	unsigned char *buffer;
	auto sz = FS_ReadFile(path, (void**)&buffer);

	if (sz == -1) {
		Con_Error(ERR_GAME, "Couldn't read image %s", path);
		return nullptr;
	}

	Image * img = new Image();

	int imgBpp;
	unsigned char *loaded = stbi_load_from_memory(buffer, sz, &img->w, &img->h, &imgBpp, 0);

	free(buffer);

	if (loaded == nullptr) {
		Con_Error(ERR_GAME, "failed to decode PNG %s", path);
		return nullptr;
	}

	unsigned int format = 0;
	if (imgBpp == 1) format = UNCOMPRESSED_GRAYSCALE;
	else if (imgBpp == 2) format = UNCOMPRESSED_GRAY_ALPHA;
	else if (imgBpp == 3) format = UNCOMPRESSED_R8G8B8;
	else if (imgBpp == 4) format = UNCOMPRESSED_R8G8B8A8;

	unsigned int tex = rlLoadTexture(loaded, img->w, img->h, format, 1);

	stbi_image_free(loaded);

	if (tex == 0) {
		Con_Error(ERR_GAME, "couldn't upload texture %s", path);
		return nullptr;
	}

	if (flags & IMAGEFLAGS_LINEAR_FILTER) {
		rlTextureParameters(tex, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
		rlTextureParameters(tex, RL_TEXTURE_MIN_FILTER, RL_FILTER_LINEAR);
	} else {
		rlTextureParameters(tex, RL_TEXTURE_MAG_FILTER, RL_FILTER_NEAREST);
		rlTextureParameters(tex, RL_TEXTURE_MIN_FILTER, RL_FILTER_NEAREST);		
	}

	img->hnd = tex;

	return img;
}

void* Img_Load(Asset &asset) {
	Image *img = Img_LoadPath(asset.path, asset.flags);
	return (void*) img;
}

void Img_Free(Asset &asset) {
	Image* img = (Image*)asset.resource;

	rlDeleteTextures(img->hnd);
	delete img;
}

void Img_ParseINI(Asset &asset, ini_t *ini) {
	int linearFilter = 0;
	ini_sget(ini, asset.name, "linearfilter", "%i", &linearFilter);
	if (linearFilter > 0) {
		asset.flags = IMAGEFLAGS_LINEAR_FILTER;
	}
}

Image* Get_Img(AssetHandle id) {
	Asset* asset = Asset_Get(ASSET_IMAGE, id);
	assert(asset != nullptr && asset->resource != nullptr);
	return (Image*) asset->resource;
}