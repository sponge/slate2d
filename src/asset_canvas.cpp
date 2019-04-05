#include "assetloader.h"
#include "external/rlgl.h"
#include "console.h"
#include <imgui.h>

void * Canvas_Load(Asset & asset) {
	// canvases need to be setup before load.
	if (asset.resource == nullptr) {
		Con_Errorf(ERR_FATAL, "canvas not setup before load %s", asset.name);
		return nullptr;
	}

	auto *canvas = (Canvas*)asset.resource;

	canvas->texture = rlLoadRenderTexture(canvas->w, canvas->h);

	if (asset.flags & IMAGEFLAGS_LINEAR_FILTER) {
		rlTextureParameters(canvas->texture.texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_LINEAR);
		rlTextureParameters(canvas->texture.texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_LINEAR);
	} else {
		rlTextureParameters(canvas->texture.texture.id, RL_TEXTURE_MAG_FILTER, RL_FILTER_NEAREST);
		rlTextureParameters(canvas->texture.texture.id, RL_TEXTURE_MIN_FILTER, RL_FILTER_NEAREST);		
	}

	return (void*)canvas;
}

void Canvas_Set(AssetHandle id, int width, int height) {
	Asset *asset = Asset_Get(ASSET_CANVAS, id);

	if (asset == nullptr) {
		Con_Error(ERR_GAME, "asset not found");
		return;
	}

	if (asset->loaded == true) {
		Con_Printf("WARNING: trying to set already loaded asset\n");
		return;
	}

	if (width == 0 || height == 0) {
		Con_Errorf(ERR_GAME, "canvas %s width and height must be > 0", asset->name);
		return;
	}

	auto *canvas = new Canvas();
	canvas->w = width;
	canvas->h = height;
	asset->resource = canvas;
}

void Canvas_Free(Asset & asset) {
	Canvas* canvas = reinterpret_cast<Canvas*>(asset.resource);
	rlDeleteRenderTextures(canvas->texture);
	delete(canvas);
}

void Canvas_ParseINI(Asset &asset, ini_t *ini) {
	int width = 0, height = 0;
	ini_sget(ini, asset.name, "width", "%i", &width);
	ini_sget(ini, asset.name, "height", "%i", &height);

	Canvas_Set(asset.id, width, height);
}

void Canvas_Inspect(Asset& asset, bool deselected)
{
	static int zoom = 1;

	Canvas* canvas = (Canvas*)asset.resource;

	ImGui::Text("Size: %ix%i", canvas->w, canvas->h);
	ImGui::SliderInt("Zoom", &zoom, 1, 8, "%dx");
	ImGui::BeginChildFrame(ImGui::GetID("inspector value"), ImVec2(0, 0), ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::Image((ImTextureID)canvas->texture.texture.id, ImVec2(canvas->w * zoom, canvas->h * zoom), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChildFrame();
}