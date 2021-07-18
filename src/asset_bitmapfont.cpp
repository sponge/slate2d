#include "files.h"
#include "slate2d.h"
#include "console.h"
#include "stb_image.h"
#include "assetloader.h"
#include "rendercommands.h"
#include "rlgl.h"
#include "external/fontstash.h"
#include "external/gl3corefontstash.h"
#include <imgui.h>

static void* bitmap_loadFont(FONScontext *context, unsigned char *data, int dataSize) {
	return data;
}

static void bitmap_freeFont(void *usrdata) {
	Con_Printf("%s\n", "WARNING: bitmap_freeFont not implemented yet");
}

static void bitmap_getFontVMetrics(void *usrdata, int *ascent, int *descent, int *lineGap) {
	BitmapFont_t *font = (BitmapFont_t*) usrdata;

	*ascent = font->h;
	*descent = 0;
	*lineGap = font->h * font->lineHeight;
}

static float bitmap_getPixelHeightScale(void *usrdata, float size) {
	return size;
}

static int bitmap_getGlyphIndex(void *usrdata, int codepoint) {
	if (codepoint > 255) {
		Con_Printf("WARNING: codepoint %i > 255", codepoint);
		return 0;
	}

	// ugh, add 1 here because 0 is considered failure, but 0 is also a valid glyph position
	return codepoint + 1;
}

static int bitmap_buildGlyphBitmap(void *usrdata, int glyph, float size, float scale, int *advance, int *lsb, int *x0, int *y0, int *x1, int *y1) {
	BitmapFont_t *font = (BitmapFont_t*) usrdata;
	BitmapGlyph &bglyph = font->offsets[--glyph];

	// this is misleading: advance should be the glyph width.
	// if x0 is set to start and x1 is set to end, it breaks rendering
	*advance = bglyph.end - bglyph.start;
	*lsb = 0;
	*x0 = 0;
	*x1 = bglyph.end - bglyph.start;
	*y0 = 0;
	*y1 = font->h;

	// handle spaces by making it take up no room and just advance by the asset's spcaewidth
	if (glyph == 32) {
		*advance = font->spaceWidth;
		*x1 = 0;
		*y1 = 0;
	}

	return 1;
}

static void bitmap_renderGlyphBitmap(void *usrdata, FONScolor *output, int outWidth, int outHeight, int outStride, float scaleX, float scaleY, int glyph) {
	BitmapFont_t *font = (BitmapFont_t*) usrdata;
	BitmapGlyph &bglyph = font->offsets[--glyph];
	
	for (int y = 0; y < outHeight; ++y) {
		for (int x = 0; x < outWidth; ++x) {
				unsigned char *offset = &font->img[(y * font->w + x + bglyph.start) * 4];

				output[y * outStride + x].r = *offset;
				output[y * outStride + x].g = *(offset + 1);
				output[y * outStride + x].b = *(offset + 2);
				output[y * outStride + x].a = *(offset + 3);
		}
	}
}


static int bitmap_getGlyphKernAdvance(void *usrdata, int glyph1, int glyph2) {
	return 0;
}

static int bitmap_engineSupportsScaling(void *usrdata) {
	return 0;
}

FONSfontEngine bmpfntEngine = {
	bitmap_loadFont,
	bitmap_freeFont,
	bitmap_getFontVMetrics,
	bitmap_getPixelHeightScale,
	bitmap_getGlyphIndex,
	bitmap_buildGlyphBitmap,
	bitmap_renderGlyphBitmap,
	bitmap_getGlyphKernAdvance,
	bitmap_engineSupportsScaling
};

void* BMPFNT_Load(Asset &asset) {
	// bitmap fonts need to be setup before load.
	if (asset.resource == nullptr) {
		Con_Errorf(ERR_FATAL, "bitmap font not setup before load %s", asset.path);
		return nullptr;
	}

	if (ctx == nullptr) {
		ctx = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	}

	void *buffer;
	auto sz = FS_ReadFile(asset.path, &buffer);

	if (sz == -1) {
		Con_Errorf(ERR_FATAL, "Failed to find file %s", asset.path);
	}

	int w, h, ch;
	auto img = stbi_load_from_memory((stbi_uc*)buffer, sz, &w, &h, &ch, 0);

	free(buffer);

	if (img == nullptr) {
		Con_Errorf(ERR_FATAL, "Failed to load bmpfont %s", asset.path);
		return nullptr;
	}

	if (ch != 4) {
		stbi_image_free(img);
		Con_Errorf(ERR_FATAL, "Bitmap font %s does not have 4 components", asset.path);
		return nullptr;
	}

	int currGlyph = 0;
	bool foundStart = false;

	BitmapFont *font = (BitmapFont*)asset.resource;

	if (font->glyphWidth > 0) {
		while (font->glyphs[currGlyph] != 0) {
			uint8_t glyph = (uint8_t)font->glyphs[currGlyph];
			font->offsets[glyph].start = currGlyph * font->glyphWidth;
			font->offsets[glyph].end = (currGlyph + 1) * font->glyphWidth;

			currGlyph++;
		}

		goto end;
	}

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			// find the alpha channel of the current pixel
			stbi_uc b = img[(y*w + x) * 4 + 3];
			if (b > 0) {
				if (foundStart == false) {
					// if it's not transparent, and we haven't started the next offset yet, start it now
					foundStart = true;
					font->offsets[(uint8_t)font->glyphs[currGlyph]].start = x;
					break;
				}
				else {
					break;
				}
			}
			else if (y == h - 1 && foundStart == true) {
				// if we're currently tracking a letter, and we found a col full of blank, the last row is where it ends
				font->offsets[(uint8_t)font->glyphs[currGlyph]].end = x;
				currGlyph++;
				foundStart = false;
				// if there are no more glyphs left to track, we've reached the end
				if (font->glyphs[currGlyph] == 0) {
					goto end;
				}
			}
		}
	}

end:

	font->img = img;
	font->w = w;
	font->h = h;

	font->hnd = fonsAddFontMemWithEngine(ctx, asset.name, (unsigned char *)asset.resource, sizeof(asset.resource), 0, &bmpfntEngine);

	return (void*)asset.resource;
}

void BMPFNT_Free(Asset &asset) {
	BitmapFont *font = (BitmapFont*)asset.resource;

	stbi_image_free(font->img);
	delete font;
}

void BMPFNT_Reload(Asset &asset) {
	BitmapFont_t *fnt = (BitmapFont_t*) asset.resource;
	
	// copy over the old settings out so we can restore them after
	char glyphs[sizeof(fnt->glyphs)];
	memcpy(glyphs, &fnt->glyphs, sizeof(fnt->glyphs));
	int glyphWidth = fnt->glyphWidth;
	int charSpacing = fnt->charSpacing;
	int spaceWidth = fnt->spaceWidth;
	int lineHeight = fnt->lineHeight;

	Asset_Unload(asset.id);
	BMPFNT_Set(asset.id, glyphs, glyphWidth, charSpacing, spaceWidth, lineHeight);
	Asset_Load(asset.id);
	fonsResetAtlas(ctx, 512, 512);
}

void BMPFNT_ParseINI(Asset &asset, ini_t *ini) {
	int glyphWidth = 0, charSpacing = 0 , spaceWidth = 0 , lineHeight = 0;
	
	const char * glyphs = ini_get(ini, asset.name, "glyphs");
	if (glyphs == nullptr) {
		Con_Errorf(ERR_FATAL, "missing key glyphs for asset %s", asset.name);
		return;
	}

	ini_sget(ini, asset.name, "glyphwidth", "%i", &glyphWidth);
	ini_sget(ini, asset.name, "charspacing", "%i", &charSpacing);
	ini_sget(ini, asset.name, "spacewidth", "%i", &spaceWidth);
	ini_sget(ini, asset.name, "lineheight", "%i", &lineHeight);

	BMPFNT_Set(asset.id, glyphs, glyphWidth, charSpacing, spaceWidth, lineHeight);
}

void BMPFNT_Set(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight) {
	Asset *asset = Asset_Get(ASSET_BITMAPFONT, assetHandle);

	if (asset == nullptr) {
		Con_Error(ERR_FATAL, "asset not found");
		return;
	}

	if (asset->loaded == true) {
		Con_Printf("WARNING: trying to set already loaded font\n");
		return;
	}

	auto font = new BitmapFont();
	
	size_t len = strlen(glyphs);
	len = len > 256 ? 256 : len;
	memcpy(font->glyphs, glyphs, len);
	font->charSpacing = charSpacing;
	font->glyphWidth = glyphWidth;
	font->spaceWidth = spaceWidth;
	font->lineHeight = lineHeight;
	
	asset->resource = (void*)font;
}

void BMPFNT_Inspect(Asset& asset, bool deselected) {
	static int zoom = 1;
	static Image* img = nullptr;
	static int currentGlyph;

	// we need to load the entire image into the gpu since fontstash handles this normally
	// if we're deselecting, free that texture free
	if (deselected && img != nullptr) {
		rlUnloadTexture(img->hnd);
		delete img;
		img = nullptr;
		currentGlyph = 0;
		return;
	}

	BitmapFont* fnt = (BitmapFont*)asset.resource;

	char glyphChar[2] = "";
	glyphChar[0] = fnt->glyphs[currentGlyph];

	ImGui::Text("Size: %ix%i", fnt->w, fnt->h);
	ImGui::Text("Glyphs: \"%s\"", fnt->glyphs);
	fnt->glyphWidth == 0 ? ImGui::Text("Glyph Width: variable") : ImGui::Text("Glyph Width: %ipx", fnt->glyphWidth);
	ImGui::Text("Character Spacing: %ipx", fnt->charSpacing);
	ImGui::Text("Line Height: %ipx", fnt->lineHeight);
	ImGui::Text("Space Width: %ipx", fnt->spaceWidth);

	// unload the old display texture if reloading
	if (ImGui::Button("Reload")) {
          rlUnloadTexture(img->hnd);
		delete img;
		img = nullptr;
		BMPFNT_Reload(asset);
		fnt = (BitmapFont*)asset.resource;
	}

	// if we're newly clicking on this item, or have reloaded, load the image into the gpu
	if (img == nullptr) {
		img = Img_LoadPath(asset.path, asset.flags);
	}

	ImGui::SameLine();
	ImGui::SliderInt("Zoom", &zoom, 1, 8, "%dx");

	// preview glyph dropdown. make a string with 2 chars, the char and a null, so we can
	// treat it like a string
	ImGui::PushItemWidth(64);
	if (ImGui::BeginCombo("Show Glyph", &glyphChar[0])) {
		for (int i = 0; i < sizeof(fnt->glyphs); i++) {
			if (fnt->glyphs[i] == '\0') {
				continue;
			}

			char title[2] = "";
			title[0] = fnt->glyphs[i];
			if (ImGui::Selectable(&title[0], i == currentGlyph)) {
				currentGlyph = i;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::BeginChildFrame(ImGui::GetID("inspector value"), ImVec2(0, 0), ImGuiWindowFlags_HorizontalScrollbar);
	BitmapGlyph &offs = fnt->offsets[fnt->glyphs[currentGlyph]];

	// figure out color and coords for glyph selection rect. need to offset by the
	// window position since we're adding to the draw list directly.
	ImU32 color = ImGui::ColorConvertFloat4ToU32(ImVec4(1, 0, 0, 1));
	ImVec2 w = ImGui::GetCursorScreenPos();
	ImVec2 top = ImVec2(w.x + offs.start * zoom, w.y);
	ImVec2 bottom = ImVec2(w.x + offs.end * zoom, w.y + fnt->h * zoom);
	ImGui::GetWindowDrawList()->AddRectFilled(top, bottom, color);

	ImGui::Image((ImTextureID)(uintptr_t)img->hnd, ImVec2(img->w * zoom, img->h * zoom));
	ImGui::EndChildFrame();
}