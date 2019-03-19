#include "assetloader.h"
#include "console.h"
#include "external/vec.h"

typedef vec_t(Asset) asset_vec_t;
asset_vec_t assets;

typedef struct {
	void*(*Load)(Asset &asset);
	void(*Free)(Asset &asset);
} AssetLoadHandler_t;

static AssetLoadHandler_t assetHandler[ASSET_MAX] = {
	{}, // ASSET_ANY
	{Img_Load, Img_Free},
	{Sprite_Load, Sprite_Free},
	{Speech_Load, Speech_Free},
	{Sound_Load, Sound_Free},
	{Sound_Load, Mod_Free},
	{TTF_Load, TTF_Free},
	{BMPFNT_Load, BMPFNT_Free},
	{TileMap_Load, TileMap_Free},
	{Canvas_Load, Canvas_Free},
	{Shader_Load, Shader_Free},
};

AssetHandle Asset_Find(const char *name) {
	for (int i = 0; i < assets.length; i++) {
		Asset *asset = &assets.data[i];
		if (strcmp(asset->name, name) == 0) {
			return asset->id;
		}
	}

	return INVALID_ASSET;
}

Asset* Asset_Get(AssetType_t type, AssetHandle id) {
	if (id >= assets.length) {
		return nullptr;
	}

	Asset* asset = &assets.data[id];
	if (type != ASSET_ANY && type != asset->type) {
		return nullptr;
	}

	return asset;
}

AssetHandle Asset_Create(AssetType_t assetType, const char *name, const char *path, int flags) {
	if (assets.data == nullptr) {
		vec_init(&assets);
		vec_reserve(&assets, 256);
	}

	if (assetType < 0 || assetType > ASSET_MAX) {
		Con_Printf("asset_create: assetType out of range name:%s path:%s", name, path);
		return INVALID_ASSET;
	}

	Con_Printf("asset_create: %s name:%s path:%s\n", assetStrings[assetType], name, path);


	AssetHandle found = Asset_Find(name);
	if (found != INVALID_ASSET) {
		return found;
	}

	Asset asset = {0};
	asset.id = assets.length;
	asset.type = assetType;
	asset.flags = flags;
	strncpy(asset.path, path, sizeof(asset.path));
	strncpy(asset.name, name, sizeof(asset.name));

	vec_push(&assets, asset);
	
	return asset.id;
}

void Asset_Load(AssetHandle i) {
	Asset &asset = assets.data[i];
	if (asset.type == ASSET_ANY || asset.loaded) {
		return;
	}

	Con_Printf("asset_load: %s name:%s path:%s\n", assetStrings[asset.type], asset.name, asset.path);
	void *resourcePtr = assetHandler[asset.type].Load(asset);
	if (resourcePtr == nullptr) {
		Con_Error(ERR_FATAL, "asset_loadall: got nullptr while loading %s", asset.name);
		return;
	}
	asset.resource = resourcePtr;
	asset.loaded = true;
}

void Asset_LoadAll() {
	for (int i = 0; i < assets.length; i++) {
		Asset_Load(i);
	}
}

void Asset_ClearAll() {
	for (int i = 0; i < assets.length; i++) {
		Asset &asset = assets.data[i];
		if (asset.loaded == false) {
			continue;
		}
		assetHandler[asset.type].Free(asset);
		asset = {0};
	}

	vec_clear(&assets);
}