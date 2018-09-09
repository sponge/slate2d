#include "assetloader.h"
#include "external/rlgl.h"
#include "console/console.h"

void * Canvas_Load(Asset & asset) {
	// canvases need to be setup before load.
	if (asset.resource == nullptr) {
		Com_Error(ERR_FATAL, "Canvas_Load: canvas not setup before load %s", asset.name);
	}

	auto *canvas = (Canvas*)asset.resource;

	canvas->texture = rlLoadRenderTexture(canvas->w, canvas->h);

	if ((asset.flags & IMAGEFLAGS_LINEAR_FILTER) == false) {
		rlTextureParameters(canvas->texture.texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_NEAREST);
		rlTextureParameters(canvas->texture.texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_NEAREST);
	}

	return (void*)canvas;
}

void Canvas_Set(AssetHandle id, int width, int height) {
	Asset *asset = Asset_Get(ASSET_CANVAS, id);

	if (asset == nullptr) {
		Com_Error(ERR_DROP, "Sprite_Set: asset not found");
		return;
	}

	if (asset->loaded == true) {
		Com_Printf("WARNING: Asset_Set: trying to set already loaded asset\n");
		return;
	}

	auto *canvas = new Canvas();
	canvas->w = width;
	canvas->h = height;
	asset->resource = canvas;
}

void Canvas_Free(Asset & asset) {
	delete(asset.resource);
}

