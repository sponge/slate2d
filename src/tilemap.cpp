#include "assetloader.h"
#include "files.h"
#include <tmx.h>

void * tmx_img_load(const char *path) {
	const char *fullpath = va("maps/%s", path);
	AssetHandle handle = Asset_Create(ASSET_IMAGE, fullpath, fullpath);
	return (void*)Asset_Get(ASSET_IMAGE, handle);
}

void tmx_img_free(void *address) {
	// FIXME: should i do this? other things could be using the asset
}

void *tmx_fs(const char *filename, int *outSz) {
	void *xml;

	*outSz = FS_ReadFile(va("maps/%s", filename), &xml);

	if (*outSz < 0) {
		Com_Error(ERR_DROP, "Couldn't load file while parsing map %s", filename);
		return nullptr;
	}

	return xml;
}

void * TileMap_Load(Asset &asset) {
	tmx_img_load_func = &tmx_img_load;
	tmx_img_free_func = &tmx_img_free;
	tmx_file_read_func = &tmx_fs;

	tmx_map *map;

	const char *xml;
	int outSz = FS_ReadFile(asset.path, (void **)&xml);
	if (outSz < 0) {
		Com_Error(ERR_DROP, "Couldn't read map %s", asset.path);
		return nullptr;
	}

	map = tmx_load_buffer(xml, outSz);

	if (map == nullptr) {
		Com_Error(ERR_DROP, "Failed to load tmx");
		return nullptr;
	}

	if (map->orient != O_ORT) {
		Com_Error(ERR_DROP, "Non orthagonal tiles not supported");
		return nullptr;
	}

	return (void*) map;
}

void TileMap_Free(Asset &asset) {
	tmx_map_free((tmx_map*) asset.resource);
}

tmx_map* Get_TileMap(AssetHandle id) {
	Asset *asset = Asset_Get(ASSET_TILEMAP, id);
	assert(asset != nullptr && asset->resource != nullptr);
	return (tmx_map*)asset->resource;
}