#include "rlgl.h"
#include "assetloader.h"
#include "files.h"
#include "console.h"
#include <physfs.h>
#include <imgui.h>

uint8_t ReadByte(uint8_t **curr) {
	uint8_t ret = (uint8_t)**curr;
	*curr += 1;

	return ret;
}

int16_t ReadShort(uint8_t **curr) {
	uint8_t lo = ReadByte(curr);
	uint8_t hi = ReadByte(curr);
	int16_t ret = (hi << 8) | lo;

	return ret;
}

const char* ReadString(uint8_t **curr) {
	const char *name = (const char*)*curr;
	*curr += strlen(name) + 1;

	return name;
}

static bool IsCrunchAsset(Asset& asset) {
	return strcmp("bin", FS_FileExtension(asset.path)) == 0;
}

void* Sprite_Load(Asset &asset) {
	// if the sprite ends in bin, load it through crunch, otherwise generate the sprite
	if (IsCrunchAsset(asset)) {
		// read the bin file
		uint8_t *crunch;
		int len = FS_ReadFile(asset.path, (void**)&crunch);
		uint8_t *curr = crunch;

		if (len == -1) {
			Con_Errorf(ERR_GAME, "couldn't read file %s", asset.path);
			return nullptr;
		}

		// we have a probably valid file, create the atlas
		SpriteAtlas *atlas = new SpriteAtlas();

		// read the counts from the file (numSprites is a new field compared to standard crunch)
		atlas->numImages = ReadShort(&curr);
		atlas->numSprites = ReadShort(&curr);

		atlas->images = new Image[atlas->numImages];
		atlas->sprites = new Sprite[atlas->numSprites];

		// for each texture, read the texture path and number of sprites in that image
		for (int tex = 0; tex < atlas->numImages; tex++) {
			const char *imgPath = ReadString(&curr);
			int16_t texSprites = ReadShort(&curr);

			// load the imgae into the GPU, copy it, and delete it
			Image *img = Img_LoadPath(imgPath, asset.flags);
			atlas->images[tex] = *img;
			delete img;

			Con_Printf("texture %s has %i images\n", imgPath, texSprites);

			// for each sprite in the texture, read it into our struct
			for (int i = 0; i < texSprites; i++) {
				// we don't care about the name, just print it
				const char *name = ReadString(&curr);

				// read all the sprite attributes
				atlas->sprites[i] = {
					&atlas->images[tex],
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadShort(&curr),
					ReadByte(&curr)
				};

				Sprite *spr = &atlas->sprites[i];
				Con_Printf("%s (%i): pos:(%i, %i) sz:(%i, %i)\n", name, i, spr->x, spr->y, spr->w, spr->h);
			}

		}

		free(crunch);
		asset.resource = (void*)atlas;
	}
	else {
		// create the atlas based off of fixed bounds
		SpriteAtlas *spr = (SpriteAtlas*)asset.resource;

		if (spr->spriteWidth == 0 || spr->spriteHeight == 0) {
			Con_Errorf(ERR_FATAL, "staticWidth and staticHeight for %s must not be 0 (missing Sprite_Set?)", asset.name);
		}

		Image *img = (Image*)Img_Load(asset);

		if (img == nullptr) {
			Con_Errorf(ERR_FATAL, "image %s failed to load", asset.name);
			return nullptr;
		}

		// sprites set with setSprite only ever have one texture
		spr->images = new Image[1];
		spr->images[0] = *img;

		int rows = (img->h / (spr->spriteHeight + spr->spriteMarginY));
		int cols = (img->w / (spr->spriteWidth + spr->spriteMarginX));

		delete img;

		spr->numImages = 1;
		spr->numSprites = rows * cols;
		spr->sprites = new Sprite[spr->numSprites];

		// step through each square in the grid and generate the structure for it
		for (int i = 0; i < spr->numSprites; i++) {
			spr->sprites[i] = { 0 };
			spr->sprites[i].texture = &spr->images[0];
			spr->sprites[i].x = (int16_t)((i % cols) * spr->spriteWidth);
			spr->sprites[i].y = (int16_t)((i / cols) * spr->spriteHeight);
			spr->sprites[i].w = (int16_t)spr->spriteWidth;
			spr->sprites[i].h = (int16_t)spr->spriteHeight;
		}

	}

	return asset.resource;
}

void Sprite_Free(Asset &asset) {
	SpriteAtlas *spr = (SpriteAtlas*)asset.resource;
	delete[] spr->sprites;

	for (int i = 0; i < spr->numImages; i++) {
		rlUnloadTexture(spr->images[i].hnd);
	}

	delete[] spr->images;

	delete spr;
}

void Sprite_Reload(Asset& asset) {
	if (IsCrunchAsset(asset)) {
		Asset_Unload(asset.id);
		Asset_Load(asset.id);
	}
	else {
		SpriteAtlas* spr = (SpriteAtlas*)asset.resource;

		int width = spr->spriteWidth;
		int height = spr->spriteHeight;
		int marginx = spr->spriteMarginX;
		int marginy = spr->spriteMarginY;

		Asset_Unload(asset.id);
		Sprite_Set(asset.id, width, height, marginx, marginy);
		Asset_Load(asset.id);
	}
}

void Sprite_ParseINI(Asset &asset, ini_t *ini) {
	if (IsCrunchAsset(asset)) {
		return;
	}
	
	int width = 0, height = 0, marginX = 0, marginY = 0;
	ini_sget(ini, asset.name, "width", "%i", &width);
	ini_sget(ini, asset.name, "height", "%i", &height);
	ini_sget(ini, asset.name, "marginx", "%i", &marginX);
	ini_sget(ini, asset.name, "marginy", "%i", &marginY);

	Sprite_Set(asset.id, width, height, marginX, marginY);
}

void Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY) {
	Asset *asset = Asset_Get(ASSET_SPRITE, assetHandle);

	if (asset == nullptr) {
		Con_Error(ERR_FATAL, "asset not found");
		return;
	}
	
	if (IsCrunchAsset(*asset)) {
		Con_Errorf(ERR_FATAL, "can't call set on a crunched sprite %s", asset->name);
		return;
	}

	if (width <= 0 || height <= 0) {
		Con_Errorf(ERR_FATAL, "width and height for sprite %s must be greater than 0", asset->name);
		return;
	}

	if (asset->loaded == true) {
		// Con_Printf("WARNING: Asset_Set: trying to set already loaded asset %s\n", asset->name);
		return;
	}

	auto spr = new SpriteAtlas();

	spr->spriteWidth = width;
	spr->spriteHeight = height;
	spr->spriteMarginX = marginX;
	spr->spriteMarginY = marginY;

	asset->resource = (void*)spr;
}

void Sprite_Inspect(Asset& asset, bool deselected) {
	static int zoom = 1;
	static int currentSprite = 0;

	if (deselected) {
		currentSprite = 0;
	}

	SpriteAtlas* atlas = (SpriteAtlas*)asset.resource;
	//ImGui::Text("Size: %ix%i", spr->, spr->h);

	if (ImGui::Button("Reload")) {
		Sprite_Reload(asset);
		atlas = (SpriteAtlas*)asset.resource;
	}

	ImGui::SameLine();
	ImGui::SliderInt("Zoom", &zoom, 1, 8, "%dx");

	ImGui::SliderInt("Sprite #", &currentSprite, 0, atlas->numSprites-1);

	Sprite& spr = atlas->sprites[currentSprite];

	ImGui::Text("Sprite Location: %ix%i", spr.x, spr.y);
	ImGui::Text("Sprite Size: %ix%i", spr.w, spr.h);
	ImGui::Text("Sprite Frame: (%i,%i) %ix%i", spr.framex, spr.framey, spr.framew, spr.framey);

	ImGui::BeginChildFrame(ImGui::GetID("inspector value"), ImVec2(0, 0), ImGuiWindowFlags_HorizontalScrollbar);

	// figure out color and coords for glyph selection rect. need to offset by the
	// window position since we're adding to the draw list directly.
	ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1));
	ImVec2 w = ImGui::GetCursorScreenPos();
	ImVec2 top = ImVec2(w.x + spr.x * zoom, w.y + spr.y * zoom);
	ImVec2 bottom = ImVec2(w.x + (spr.x + spr.w) * zoom, w.y + (spr.y + spr.h) * zoom);
	ImGui::GetWindowDrawList()->AddRectFilled(top, bottom, color);

	ImGui::Image((ImTextureID)(uintptr_t)spr.texture->hnd, ImVec2(spr.texture->w * zoom, spr.texture->h * zoom));
	ImGui::EndChildFrame();
}
