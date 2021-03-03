#include "main.h"
#include "assetloader.h"
#include "console.h"
#include "external/vec.h"
#include "files.h"
extern "C" {
#include "external/ini.h"
}
#include "external/fontstash.h"
#include "external/gl3corefontstash.h"
#include <imgui.h>
#include "cvar_main.h"
#include "rendercommands.h"

// when adding a new asset in slate2d.h, add the string representation here
// used for asset system debugging
static const char* assetStrings[] = {
	"ASSET_ANY",
	"ASSET_IMAGE",
	"ASSET_SPRITE",
	"ASSET_SPEECH",
	"ASSET_SOUND",
	"ASSET_MOD",
	"ASSET_FONT",
	"ASSET_BITMAPFONT",
	"ASSET_TMX",
	"ASSET_CANVAS",
	"ASSET_SHADER",
	"ASSET_MAX",
};

typedef vec_t(Asset) asset_vec_t;
asset_vec_t assets;

typedef struct {
	const char *iniType;
	int iniFlags;
	void(*ParseINI)(Asset &asset, ini_t *ini);
	void*(*Load)(Asset &asset);
	void(*Free)(Asset &asset);
	void(*Inspect)(Asset& asset, bool deselected);
} AssetLoadHandler_t;

#define INIFLAGS_OPTIONALPATH 1

static AssetLoadHandler_t assetHandler[ASSET_MAX] = {
	{}, // ASSET_ANY
	{"image", 0, Img_ParseINI, Img_Load, Img_Free, Img_Inspect },
	{"sprite", 0, Sprite_ParseINI, Sprite_Load, Sprite_Free, Sprite_Inspect },
	{"speech", INIFLAGS_OPTIONALPATH, Speech_ParseINI, Speech_Load, Speech_Free, Sound_Inspect },
	{"sound", 0, nullptr, Sound_Load, Sound_Free, Sound_Inspect },
	{"mod", 0, nullptr, Sound_Load, Mod_Free, Sound_Inspect },
	{"ttf", 0, nullptr, TTF_Load, TTF_Free },
	{"bitmapfont", 0, BMPFNT_ParseINI, BMPFNT_Load, BMPFNT_Free, BMPFNT_Inspect },
	{"tmx", 0, nullptr, TMX_Load, TMX_Free },
	{"canvas", INIFLAGS_OPTIONALPATH, Canvas_ParseINI, Canvas_Load, Canvas_Free, Canvas_Inspect },
	{"shader", INIFLAGS_OPTIONALPATH, Shader_ParseINI, Shader_Load, Shader_Free, Shader_Inspect },
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
	asset.path = strdup(path);
	asset.name = strdup(name);

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

void Asset_Unload(AssetHandle i) {
	Asset &asset = assets.data[i];
	if (asset.type == ASSET_ANY || !asset.loaded) {
		return;
	}

	Con_Printf("asset_unload: %s name:%s path:%s\n", assetStrings[asset.type], asset.name, asset.path);
	assetHandler[asset.type].Free(asset);
	asset.resource = nullptr;
	asset.loaded = false;
}

void Asset_ClearAll() {
	for (int i = 0; i < assets.length; i++) {
		Asset &asset = assets.data[i];
		if (asset.loaded == false) {
			continue;
		}
		assetHandler[asset.type].Free(asset);
		free((void*)asset.name);
		free((void*)asset.path);
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
}

void Asset_DrawInspector() {
	static int currentItem;
	int lastItem = -1;

	if (!debug_assets->boolean) {
		if (debug_assets->boolean == false && debug_assets->integer == 1) {
			Con_SetVar("debug.assets", 0);
		}

		return;
	}

	ImGui::SetNextWindowSizeConstraints(ImVec2(640, 250), ImVec2(9999, 9999));
	if (ImGui::Begin("Asset Inspector", &debug_assets->boolean)) {
		if (currentItem >= assets.length) {
			currentItem = assets.length - 1;
		}

		ImVec2 window = ImGui::GetWindowSize();

		ImGui::Columns(2);
		int width = window.x * 0.3 > 250 ? 250 : window.x * 0.3;
		ImGui::SetColumnWidth(0, width);
		ImGui::SetColumnWidth(1, window.x - width);

		if (ImGui::ListBoxHeader("##Assets", ImVec2(-0.1, -0.1)))
		{
			for (int i = 0; i < assets.length; i++) {
				if (ImGui::Selectable(assets.data[i].name, i == currentItem)) {
					lastItem = currentItem;
					currentItem = i;
				}
			}
			ImGui::ListBoxFooter();
		}

		ImGui::NextColumn();
		Asset& asset = assets.data[currentItem];
		ImGui::Text("Name: %s", asset.name);
		ImGui::Text("Type: %s", assetHandler[asset.type].iniType);
		if (asset.path[0] != '\0') {
			ImGui::Text("Path: %s", asset.path);
		}

		if (lastItem != -1) {
			Asset &lastAsset = assets.data[lastItem];
			if (assetHandler[lastAsset.type].Inspect != nullptr) {
				assetHandler[lastAsset.type].Inspect(lastAsset, true);
			}
		}
		if (assetHandler[asset.type].Inspect != nullptr) {
			assetHandler[asset.type].Inspect(asset, false);
		}
	}
	ImGui::End();
}