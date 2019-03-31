#include "assetloader.h"
#include "console.h"
#include "external/vec.h"
#include "files.h"
extern "C" {
#include "external/sds.h"
#include "external/ini.h"
}
#include "external/fontstash.h"
#include "external/gl3corefontstash.h"

typedef vec_t(Asset) asset_vec_t;
asset_vec_t assets;

extern FONScontext *ctx;

typedef struct {
	const char *iniType;
	int iniFlags;
	void(*ParseINI)(Asset &asset, ini_t *ini);
	void*(*Load)(Asset &asset);
	void(*Free)(Asset &asset);
} AssetLoadHandler_t;

#define INIFLAGS_OPTIONALPATH 1

static AssetLoadHandler_t assetHandler[ASSET_MAX] = {
	{}, // ASSET_ANY
	{"image", 0, Img_ParseINI, Img_Load, Img_Free },
	{"sprite", 0, Sprite_ParseINI, Sprite_Load, Sprite_Free },
	{"speech", INIFLAGS_OPTIONALPATH, Speech_ParseINI, Speech_Load, Speech_Free },
	{"sound", 0, nullptr, Sound_Load, Sound_Free },
	{"mod", 0, nullptr, Sound_Load, Mod_Free },
	{"ttf", 0, nullptr, TTF_Load, TTF_Free },
	{"bitmapfont", 0, BMPFNT_ParseINI, BMPFNT_Load, BMPFNT_Free },
	{"tilemap", 0, nullptr, TileMap_Load, TileMap_Free },
	{"canvas", INIFLAGS_OPTIONALPATH, Canvas_ParseINI, Canvas_Load, Canvas_Free },
	{"shader", INIFLAGS_OPTIONALPATH, Shader_ParseINI, Shader_Load, Shader_Free },
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

	if (path == nullptr) {
		path = "";
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
	asset.path = sdsnew(path);
	asset.name = sdsnew(name);

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
		Con_Errorf(ERR_FATAL, "got nullptr while loading %s", asset.name);
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
		sdsfree((sds)asset.name);
		sdsfree((sds)asset.path);
		asset = {0};
	}

	vec_clear(&assets);

	if (ctx != nullptr) {
		glfonsDelete(ctx);
		ctx = nullptr;
	}
}

void Asset_LoadINI(const char *path) {
	char *inistr;
	int sz = FS_ReadFile(path, (void**)&inistr);

	if (sz == -1) {
		Con_Errorf(ERR_FATAL, "failed to read file %s", path);
		return;
	}

	ini_t* ini = ini_load_mem(inistr, sz);

	free(inistr);

	if (ini == nullptr) {
		Con_Errorf(ERR_FATAL, "failed to parse ini %s", path);
		return;
	}

	ini_iter_t iter;
	ini_iter_init(ini, &iter);

	const char *lastSection = nullptr;
	while (ini_iter_next(&iter) != nullptr) {
		if (iter.section == lastSection) {
			continue;
		}

		Con_Printf("new section: %s\n", iter.section);
		lastSection = iter.section;

		const char *type = ini_get(ini, iter.section, "type");
		if (type == nullptr) {
			Con_Errorf(ERR_FATAL, "section %s missing type key", iter.section);
			return;
		}

		AssetType_t assetType = ASSET_ANY;

		for (int i = 1; i < ASSET_MAX; i++) {
			if (strcasecmp(type, assetHandler[i].iniType) == 0) {
				assetType = (AssetType_t) i;
			}
		}

		if (assetType == ASSET_ANY) {
			Con_Errorf(ERR_FATAL, "section %s has invalid type %s", iter.section, type);
			return;
		}

		const char *assetPath = ini_get(ini, iter.section, "path");
		if (assetPath == nullptr && (assetHandler[assetType].iniFlags & INIFLAGS_OPTIONALPATH) == false) {
			Con_Errorf(ERR_FATAL, "section %s missing path key", iter.section);
			return;
		}

		AssetHandle hnd = Asset_Create(assetType, iter.section, assetPath, 0);

		if (assetHandler[assetType].ParseINI != nullptr) {
			assetHandler[assetType].ParseINI(assets.data[hnd], ini);
		}
	}

	ini_free(ini);

	Con_Print("hello world\n");
}