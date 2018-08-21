#include "assetloader.h"
#include "console/console.h"

#define MAX_ASSETS 256
Asset assets[MAX_ASSETS];
unsigned int nextAsset = 0;

typedef struct AssetLoadHandler {
	void*(*Load)(Asset &asset);
	void(*Free)(Asset &asset);
} AssetLoadHandler_t;

static AssetLoadHandler assetHandler[ASSET_MAX] = {
	{}, // ASSET_ANY
	{Img_Load, Img_Free},
	{Sprite_Load, Sprite_Free},
	{Speech_Load, Speech_Free},
	{Sound_Load, Sound_Free},
	{Sound_Load, Mod_Free},
	{TTF_Load, TTF_Free},
	{BMPFNT_Load, BMPFNT_Free},
	{TileMap_Load, TileMap_Free},
};

AssetHandle Asset_Find(const char *name) {
	for (int i = 0; i < MAX_ASSETS; i++) {
		Asset *asset = &assets[i];
		if (strcmp(asset->name, name) == 0) {
			return asset->id;
		}
	}

	return INVALID_ASSET;
}

Asset* Asset_Get(AssetType_t type, AssetHandle id) {
	if (id > MAX_ASSETS) {
		return nullptr;
	}

	Asset* asset = &assets[id];
	if (type != ASSET_ANY && type != asset->type) {
		return nullptr;
	}

	return asset;
}

AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path, int flags) {
	Com_Printf("asset_create: %s name:%s path:%s\n", assetStrings[assetType], name, path);

	if (assetType < 0 || assetType > ASSET_MAX) {
		return INVALID_ASSET;
	}

	AssetHandle found = Asset_Find(name);
	if (found != INVALID_ASSET) {
		return found;
	}

	if (nextAsset >= MAX_ASSETS) {
		return INVALID_ASSET;
	}

	Asset *asset = &assets[nextAsset];
	asset->id = nextAsset;
	asset->type = assetType;
	asset->flags = flags;
	strncpy(asset->path, path, sizeof(asset->path));
	strncpy(asset->name, name, sizeof(asset->name));
	asset->resource = nullptr;

	nextAsset++;
	
	return asset->id;
}

void Asset_Load(AssetHandle i) {
	Asset &asset = assets[i];
	if (asset.type == ASSET_ANY || asset.loaded) {
		return;
	}

	Com_Printf("asset_load: %s name:%s path:%s\n", assetStrings[asset.type], asset.name, asset.path);
	void *resourcePtr = assetHandler[asset.type].Load(asset);
	if (resourcePtr == nullptr) {
		Com_Error(ERR_FATAL, "asset_loadall: got nullptr while loading %s", asset.name);
		return;
	}
	asset.resource = resourcePtr;
	asset.loaded = true;
}

void Asset_LoadAll() {
	for (int i = 0; i < MAX_ASSETS; i++) {
		Asset_Load(i);
	}
}

void Asset_ClearAll() {
	for (int i = 0; i < MAX_ASSETS; i++) {
		Asset &asset = assets[i];
		if (asset.loaded == false) {
			continue;
		}
		assetHandler[asset.type].Free(asset);
		asset.resource = nullptr;
	}
	memset(assets, 0, sizeof(Asset) * MAX_ASSETS);
	nextAsset = 0;
}