#include "files.h"
#include "../game/shared.h"
#include "console/console.h"
#include "stb_image.h"
#include "assetloader.h"
#include "rendercommands.h"
#include "rlgl.h"
#include "external/fontstash.h"

extern ClientInfo inf;

enum TextCodepointType {
	TEXT_SPACE,
	TEXT_NEWLINE,
	TEXT_CHAR,
	TEXT_IGNORE,
};

void* BMPFNT_Load(Asset &asset) {
	// bitmap fonts need to be setup before load.
	if (asset.resource == nullptr) {
		Com_Error(ERR_FATAL, "BMPFNT_Load: bitmap font not setup before load %s", asset.path);
		return nullptr;
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

int BMPFNT_TextWidth(AssetHandle assetHandle, const char *string, float scale, const char *end) {
	Asset *asset = Asset_Get(ASSET_BITMAPFONT, assetHandle);

	if (asset == nullptr) {
		Com_Error(ERR_DROP, "BMPFNT_TextWidth: asset not valid");
		return 0;
	}

	BitmapFont *font = (BitmapFont*)asset->resource;

	if (font == nullptr) {
		Com_Error(ERR_DROP, "BMPFNT_TextWidth: asset resource not valid");
		return 0;
	}

	if (end == nullptr) {
		end = string + strlen(string);
	}

	int currX = 0;
	int maxX = 0;

	int i = 0;
	while (&string[i] != end) {
		if (string[i] == '\n') {
			if (currX > maxX) {
				maxX = currX;
			}

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

	currX -= font->charSpacing;

	return (int) (maxX * scale);
}

int BMPFNT_DrawText(AssetHandle assetHandle, float x, float y, const char *string, const char *end) {
	Asset *asset = Asset_Get(ASSET_BITMAPFONT, assetHandle);

	if (asset == nullptr) {
		Com_Error(ERR_DROP, "BMPFNT_DrawText: asset not valid");
		return 0;
	}

	BitmapFont *font = (BitmapFont*)asset->resource;

	if (font == nullptr) {
		Com_Error(ERR_DROP, "BMPFNT_DrawText: asset resource not valid");
		return 0;
	}

	if (end == nullptr) {
		end = string + strlen(string);
	}

	float currX = x, currY = y;
	int i = 0;
	while (&string[i] != end) {
		if (string[i] == '\n') {
			currY += font->lineHeight * state.size;
			currX = x;
			i++;
			continue;
		}
		
		if (string[i] == ' ') {
			currX += font->spaceWidth * state.size;
			i++;
			continue;
		}

		BitmapGlyph &glyph = font->offsets[string[i]];

		DrawImage(currX, currY, (float) (glyph.end - glyph.start), (float)font->h, (float)glyph.start, 0.0f, 1.0f, state.size, 0, font->img->hnd, font->img->w, font->img->h);

		currX += (glyph.end - glyph.start + font->charSpacing) * state.size;

		i++;
	}

	return (int)(currX - font->charSpacing - x);
}

void BMPFNT_TextBox(const drawTextCommand_t *cmd, const char *string) {
	BitmapFont *font = (BitmapFont*)state.font->resource;

	if (font == nullptr) {
		Com_Error(ERR_DROP, "BMPFNT_BMPFNT_TextBoxDrawText: asset resource not valid");
		return;
	}

	int halign = state.align & (FONS_ALIGN_LEFT | FONS_ALIGN_CENTER | FONS_ALIGN_RIGHT);
	const char *current = string, *prev = string;
	float currWidth = 0;
	float currY = cmd->y;
	const char* lineStart = NULL;
	int type = TEXT_SPACE, ptype = TEXT_SPACE;
	bool writeLine = false;

	for (; current[0] != '\0'; ++current) {
		char currChar = current[0];
		//char prevChar = prev[0];

		switch (currChar) {
			case 9: // \t
			case 11: // \v
			case 12: // \f
			case 32: // space
			case 0x00a0: // NBSP
				type = TEXT_SPACE;
				break;
			case 10:		// \n
				type = TEXT_NEWLINE;
				break;
			case 13:		// \r
				type = TEXT_IGNORE;
				break;
			case 0x0085:	// NEL
				type = TEXT_NEWLINE;
				break;
			default:
				type = TEXT_CHAR;
				break;
		}

		if (type == TEXT_NEWLINE) {
			writeLine = true;
		}
		else if (lineStart == nullptr) {
			if (type == TEXT_CHAR) {
				lineStart = current;
			}
		}
		else if (type != TEXT_IGNORE) {
			float thisWidth = 0;
			if (type == TEXT_SPACE) {
				thisWidth = font->spaceWidth * state.size;
			}
			else {
				BitmapGlyph &glyph = font->offsets[currChar];
				thisWidth = (glyph.end - glyph.start + font->charSpacing) * state.size;
			}

			if (cmd->w > 0 && currWidth + thisWidth >= cmd->w) {
				writeLine = true;
			} else {
				currWidth += thisWidth;
			}
		}

		if (writeLine) {
			if (lineStart != nullptr) {
				float x = cmd->x;
				if (cmd->w > 0 && state.align & (FONS_ALIGN_CENTER | FONS_ALIGN_RIGHT)) {
					// FIXME: this should be unnecessary, but currWidth and width are not equal
					int width = BMPFNT_TextWidth(state.font->id, lineStart, state.size, prev);
					x += halign & FONS_ALIGN_CENTER ? (cmd->w - width) / 2 : 0;
					x += halign & FONS_ALIGN_RIGHT ? cmd->w - width : 0;
				}
				BMPFNT_DrawText(state.font->id, x, currY, lineStart, prev);
				if (type != TEXT_NEWLINE) {
					current -= 2;
				}
			}

			lineStart = nullptr;
			currWidth = 0;
			currY += font->lineHeight * state.size;
			writeLine = false;
		}

		ptype = type;
		prev = current;
	}

	if (lineStart != nullptr) {
		float x = cmd->x;

		if (cmd->w > 0 && state.align & (FONS_ALIGN_CENTER | FONS_ALIGN_RIGHT)) {
			// FIXME: this should be unnecessary, but currWidth and width are not equal
			// note: not entirely copy paste, nullptr instead of prev
			int width = BMPFNT_TextWidth(state.font->id, lineStart, state.size, nullptr);
			x += halign & FONS_ALIGN_CENTER ? (cmd->w - width) / 2 : 0;
			x += halign & FONS_ALIGN_RIGHT ? cmd->w - width : 0;
		}

		BMPFNT_DrawText(state.font->id, x, currY, lineStart, nullptr);	
	}
}
