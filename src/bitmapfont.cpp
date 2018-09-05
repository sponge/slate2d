#include "bitmapfont.h"
#include "files.h"
#include "../game/shared.h"
#include "console/console.h"
#include "stb_image.h"
#include "assetloader.h"
#include "rendercommands.h"
#include "rlgl.h"

extern ClientInfo inf;

void* BMPFNT_Load(Asset &asset) {
	// bitmap fonts need to be setup before load.
	if (asset.resource == nullptr) {
		Com_Error(ERR_FATAL, "BMPFNT_Load: bitmap font not setup before load %s", asset.path);
	}

	void *buffer;
	auto sz = FS_ReadFile(asset.path, &buffer);

	int w, h, ch;
	auto img = stbi_load_from_memory((stbi_uc*)buffer, sz, &w, &h, &ch, 0);

	if (img == nullptr) {
		Com_Error(ERR_FATAL, "Failed to load bmpfont %s", asset.path);
		return nullptr;
	}

	if (ch != 4) {
		Com_Error(ERR_FATAL, "Bitmap font %s does not have 4 components", asset.path);
		return nullptr;
	}

	int currGlyph = 0;
	bool foundStart = false;

	BitmapFont *font = (BitmapFont*)asset.resource;

	if (font->glyphWidth > 0) {
		while (font->glyphs[currGlyph] != 0) {
			byte glyph = (byte)font->glyphs[currGlyph];
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
					font->offsets[(byte)font->glyphs[currGlyph]].start = x;
					break;
				}
				else {
					break;
				}
			}
			else if (y == h - 1 && foundStart == true) {
				// if we're currently tracking a letter, and we found a col full of blank, the last row is where it ends
				font->offsets[(byte)font->glyphs[currGlyph]].end = x;
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

	font->img = (Image*) Img_Load(asset);
	font->w = w;
	font->h = h;

	free(buffer);

	return (void*)asset.resource;
}

void BMPFNT_Free(Asset &asset) {
	BitmapFont *font = (BitmapFont*)asset.resource;
	rlDeleteTextures(font->img->hnd);
	free(font->img);
	free(font);
}

void BMPFNT_Set(AssetHandle assetHandle, const char *glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight) {
	Asset *asset = Asset_Get(ASSET_BITMAPFONT, assetHandle);

	if (asset == nullptr) {
		Com_Error(ERR_FATAL, "BMPFNT_Set: asset not found");
		return;
	}

	if (asset->loaded == true) {
		Com_Printf("WARNING: BMPFNT_Set: trying to set already loaded font\n");
		return;
	}

	auto font = new BitmapFont();
	// FIXME: unsafe?
	memcpy(font->glyphs, glyphs, strlen(glyphs));
	font->charSpacing = charSpacing;
	font->glyphWidth = glyphWidth;
	font->spaceWidth = spaceWidth;
	font->lineHeight = lineHeight;
	
	asset->resource = (void*)font;
}

int BMPFNT_TextWidth(AssetHandle assetHandle, const char *string, float scale) {
	Asset *asset = Asset_Get(ASSET_BITMAPFONT, assetHandle);
	BitmapFont *font = (BitmapFont*)asset->resource;

	assert(asset != nullptr && font != nullptr);

	int currX = 0;

	int i = 0;
	while (string[i] != '\0') {
		if (string[i] == '\n') {
			currX = 0;
			i++;
			continue;
		}

		if (string[i] == ' ') {
			currX += font->spaceWidth;
			i++;
			continue;
		}

		BitmapGlyph &glyph = font->offsets[string[i]];

		currX += glyph.end - glyph.start + font->charSpacing;
		i++;

	}

	return currX * scale;
}

int BMPFNT_DrawText(AssetHandle assetHandle, float x, float y, float scale, const char *string) {
	Asset *asset = Asset_Get(ASSET_BITMAPFONT, assetHandle);
	BitmapFont *font = (BitmapFont*)asset->resource;

	assert(asset != nullptr && font != nullptr);

	float currX = x, currY = y;
	int i = 0;
	while (string[i] != '\0') {
		if (string[i] == '\n') {
			currY += font->lineHeight * scale;
			currX = x;
			i++;
			continue;
		}
		
		if (string[i] == ' ') {
			currX += font->spaceWidth * scale;
			i++;
			continue;
		}

		BitmapGlyph &glyph = font->offsets[string[i]];

		DrawImage(currX, currY, glyph.end - glyph.start, font->h, glyph.start, 0, 1.0, scale, 0, font->img->hnd, font->img->w, font->img->h);

		currX += (glyph.end - glyph.start + font->charSpacing) * scale;

		i++;
	}

	return currX - font->charSpacing - x;
}
