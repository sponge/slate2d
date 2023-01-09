#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <imgui.h>

Image* Img_LoadPath(const char *path, int flags) {
	unsigned char *buffer;
	auto sz = FS_ReadFile(path, (void**)&buffer);

	if (sz == -1) {
		Con_Errorf(ERR_GAME, "Couldn't read image %s", path);
		return nullptr;
	}

	Image * img = new Image();

	int imgBpp;
	unsigned char *loaded = stbi_load_from_memory(buffer, sz, &img->w, &img->h, &imgBpp, 0);

	free(buffer);

	if (loaded == nullptr) {
		Con_Errorf(ERR_GAME, "failed to decode PNG %s", path);
		return nullptr;
	}

	unsigned int format = 0;
	if (imgBpp == 1) format = RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
	else if (imgBpp == 2) format = RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
	else if (imgBpp == 3) format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8;
	else if (imgBpp == 4) format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

	unsigned int tex = rlLoadTexture(loaded, img->w, img->h, format, 1);

	stbi_image_free(loaded);

	if (tex == 0) {
		Con_Errorf(ERR_GAME, "couldn't upload texture %s", path);
		return nullptr;
	}

	if (flags) {
		rlTextureParameters(tex, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_BILINEAR);
		rlTextureParameters(tex, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_BILINEAR);
	} else {
		rlTextureParameters(tex, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_POINT);
		rlTextureParameters(tex, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_POINT);		
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

	rlUnloadTexture(img->hnd);
	delete img;
}

void Img_Reload(Asset &asset) {
		Asset_Unload(asset.id);
		Asset_Load(asset.id);
}

void Img_ParseINI(Asset &asset, ini_t *ini) {
	int linearFilter = 0;
	ini_sget(ini, asset.name, "linearfilter", "%i", &linearFilter);
	if (linearFilter > 0) {
		asset.flags = 1;
	}
}

const Image* Get_Img(AssetHandle id) {
	Asset* asset = Asset_Get(ASSET_IMAGE, id);
	assert(asset != nullptr && asset->resource != nullptr);
	return (const Image*) asset->resource;
}

Dimensions SLT_Get_ImgSize(AssetHandle id) {
  const Image *img = Get_Img(id);

  return {img->w, img->h};
}

void Img_Inspect(Asset& asset, bool deselected) {
	static int zoom = 1;

	Image* img = (Image*)asset.resource;
	ImGui::Text("Size: %ix%i", img->w, img->h);

	if (ImGui::Button("Reload")) {
		Img_Reload(asset);
		img = (Image*)asset.resource;
	}

	ImGui::SameLine();
	ImGui::SliderInt("Zoom", &zoom, 1, 8, "%dx");
	ImGui::BeginChildFrame(ImGui::GetID("inspector value"), ImVec2(0, 0), ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::Image((ImTextureID)(uintptr_t)img->hnd, ImVec2(img->w * zoom, img->h * zoom));
	ImGui::EndChildFrame();
}
