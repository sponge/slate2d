#include <vector>
#include "bitmapfont.h"
#include "files.h"
#include "../game/shared.h"
#include "console/console.h"
#include <stb_image.h>

// FIXME: work out how to do this with a vector of structs instead vector of ptrs
std::vector<BitmapFont *> fonts;
extern ClientInfo inf;

BitmapFont * BMPFNT_Create(const char *name, const char *path, const char *glyphs, int charSpacing, int spaceWidth, int lineHeight) {
	Com_Printf("bmpfont_create: %s %s\n", name, path);
	auto found = BMPFNT_Find(name);
	if (found != nullptr) {
		return found;
	}

	auto font = new BitmapFont();

	font->index = fonts.size();
	strncpy(font->path, path, sizeof(font->path));
	strncpy(font->name, name, sizeof(font->name));
	memcpy_s(font->glyphs, sizeof(font->glyphs), glyphs, strlen(glyphs));
	font->charSpacing = charSpacing;
	font->spaceWidth = spaceWidth;
	font->lineHeight = lineHeight;
	
	fonts.push_back(font);
	return font;
}

void BMPFNT_Load(BitmapFont &font) {
	Com_Printf("bmpfont_load: %s %s\n", font.name, font.path);

	void *buffer;
	auto sz = FS_ReadFile(font.path, &buffer);

	int w, h, ch;
	auto img = stbi_load_from_memory((stbi_uc*)buffer, sz, &w, &h, &ch, 0);

	if (img == nullptr) {
		Com_Error(ERR_FATAL, "Failed to load bmpfont %s", font.path);
		return;
	}

	if (ch != 4) {
		Com_Error(ERR_FATAL, "Bitmap font %s does not have 4 components", font.path);
		return;
	}

	int currGlyph = 0;
	bool foundStart = false;

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			// find the alpha channel of the current pixel
			stbi_uc b = img[(y*w + x) * 4 + 3];
			if (b > 0) {
				if (foundStart == false) {
					// if it's not transparent, and we haven't started the next offset yet, start it now
					foundStart = true;
					font.offsets[(byte)font.glyphs[currGlyph]].start = x;
					break;
				}
				else {
					break;
				}
			}
			else if (y == h - 1 && foundStart == true) {
				// if we're currently tracking a letter, and we found a col full of blank, the last row is where it ends
				font.offsets[(byte)font.glyphs[currGlyph]].end = x;
				currGlyph++;
				foundStart = false;
				// if there are no more glyphs left to track, we've reached the end
				if (font.glyphs[currGlyph] == 0) {
					goto end;
				}
			}
		}
	}

	end:
	font.nvg = inf.nvg;
	font.hnd = nvgCreateImageMem(font.nvg, NVG_IMAGE_NEAREST, (unsigned char *)buffer, sz);
	font.w = w;
	font.h = h;

	free(buffer);
}

BitmapFont * BMPFNT_Get(unsigned int i) {
	return fonts[i];
}

BitmapFont * BMPFNT_Find(const char *name) {
	for (auto font : fonts) {
		if (strcmp(font->name, name) == 0) {
			return font;
		}
	}

	return nullptr;
}

void BMPFNT_LoadAll() {
	for (auto font : fonts) {
		if (font->hnd == 0) {
			BMPFNT_Load(*font);
		}
	}
}

bool BMPFNT_Free(const char *name) {
	Com_Printf("bmpfnt_free: trying to free %s... ", name);
	for (auto font : fonts) {
		if (strcmp(font->name, name) == 0) {
			nvgDeleteImage(font->nvg, font->hnd);
			font->hnd = 0;
			font->nvg = nullptr;
			Com_Printf("found!\n");
			return true;
		}
	}

	Com_Printf("not found!\n");
	return false;
}

void BMPFNT_Clear() {
	for (auto font : fonts) {
		nvgDeleteImage(font->nvg, font->hnd);
	}
	fonts.clear();
}

int BMPFNT_TextWidth(BitmapFont &font, const char *string) {
	int currX = 0;

	int i = 0;
	while (string[i] != '\0') {
		if (string[i] == '\n') {
			currX = 0;
			i++;
			continue;
		}

		if (string[i] == ' ') {
			currX += font.spaceWidth;
			i++;
			continue;
		}

		BitmapGlyph &glyph = font.offsets[string[i]];

		currX += glyph.end - glyph.start + font.charSpacing;
		i++;

	}

	return currX;
}

int BMPFNT_DrawText(BitmapFont &font, float x, float y, float scale, const char *string) {
	float currX = x, currY = y;
	int i = 0;
	while (string[i] != '\0') {
		if (string[i] == '\n') {
			currY += font.lineHeight;
			currX = x;
			i++;
			continue;
		}
		
		if (string[i] == ' ') {
			currX += font.spaceWidth;
			i++;
			continue;
		}

		BitmapGlyph &glyph = font.offsets[string[i]];

		nvgSave(inf.nvg);
		nvgScale(inf.nvg, scale, scale);

		nvgTranslate(inf.nvg, currX, currY);

		auto paint = nvgImagePattern(inf.nvg, 0 - glyph.start, 0, font.w, font.h, 0, font.hnd, 1.0);
		nvgBeginPath(inf.nvg);
		nvgRect(inf.nvg, 0, 0, glyph.end - glyph.start, font.h);
		nvgFillPaint(inf.nvg, paint);
		nvgFill(inf.nvg);

		nvgRestore(inf.nvg);

		currX += glyph.end - glyph.start + font.charSpacing;

		i++;
	}

	return currX - font.charSpacing - x;
}