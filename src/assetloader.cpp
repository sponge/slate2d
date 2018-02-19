#include "assetloader.h"
#include "console/console.h"
#include <vector>

std::vector<Asset> assets;

typedef struct AssetLoadHandler {
	void*(*Load)(Asset &asset);
	void(*Free)(Asset &asset);
} AssetLoadHandler_t;

static AssetLoadHandler assetHandler[ASSET_MAX] = {
	{Img_Load, Img_Free}
};

AssetHandle Asset_Find(const char *name) {
	for (auto asset : assets) {
		if (strcmp(asset.name, name) == 0) {
			return asset.id;
		}
	}

	return INVALID_ASSET;
}

Asset* Asset_Get(AssetHandle id) {
	return &assets.at(id);
}

AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path) {
	Com_Printf("asset_create: %i %s %s\n", assetType, name, path);

	if (assetType < 0 || assetType > ASSET_MAX) {
		return INVALID_ASSET;
	}

	AssetHandle found = Asset_Find(name);
	if (found != INVALID_ASSET) {
		return found;
	}

	Asset asset;
	asset.id = assets.size();
	strncpy(asset.path, path, sizeof(asset.path));
	strncpy(asset.name, name, sizeof(asset.name));
	assets.push_back(asset);

	return asset.id;
}

void Asset_LoadAll() {
	for (auto &asset : assets) {
		void *resourcePtr = assetHandler[asset.type].Load(asset);
		if (resourcePtr == nullptr) {
			Com_Error(ERR_DROP, "asset_loadall: got nullptr while loading %s", asset.name);
			return;
		}
		asset.resource = resourcePtr;
	}
}

void Asset_ClearAll() {
	for (auto asset : assets) {
		assetHandler[asset.type].Free(asset);
	}

	assets.clear();
}