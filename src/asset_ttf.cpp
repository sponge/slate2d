#include <fontstash.h>
#include "assetloader.h"
#include "files.h"
#include "console/console.h"
#include "external/fontstash.h"
#include "external/gl3corefontstash.h"
#include "rendercommands.h"

FONScontext *ctx;

extern ClientInfo inf;

enum TTFcodepointType {
	TTF_SPACE,
	TTF_NEWLINE,
	TTF_CHAR,
	TTF_CJK_CHAR,
};

struct TTFtextRow {
	const char* start;	// Pointer to the input text where the row starts.
	const char* end;	// Pointer to the input text where the row ends (one past the last character).
	const char* next;	// Pointer to the beginning of the next row.
	float width;		// Logical width of the row.
	float minx, maxx;	// Actual bounds of the row. Logical with and bounds can differ because of kerning and some parts over extending.
};
typedef struct TTFtextRow TTFtextRow;

// can't unload TTFs, so we'll need to make sure we don't keep reloading them, sadly
void* TTF_Load(Asset &asset) {
	TTFFont_t *fnt = new TTFFont_t();

	if (ctx == nullptr) {
		ctx = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	}

	int found = fonsGetFontByName(ctx, asset.name);
	if (found != FONS_INVALID) {
		fnt->valid = true;
		fnt->hnd = found;
		found++;
		return fnt;
	}

	unsigned char *font;
	auto sz = FS_ReadFile(asset.path, (void **)&font);

	if (sz == -1) {
		Com_Error(ERR_DROP, "TTF_Load: couldn't load file %s", asset.path);
		return nullptr;
	}

	int hnd = fonsAddFontMem(ctx, asset.name, font, sz, 1);
	if (hnd < 0) {
		return nullptr;
	}

	fnt->valid = true;
	fnt->hnd = hnd;

	return (void*)fnt;
}

void TTF_Free(Asset &asset) {
	// doesn't delete from fontstash, just our object
	TTFFont_t *fnt = (TTFFont_t*)asset.resource;
	delete fnt;
}

int TTF_BreakLines(const char *string, const char *end, float breakRowWidth, TTFtextRow* rows, int maxRows) {
	FONStextIter iter, prevIter;
	FONSquad q;
	int nrows = 0;
	float rowStartX = 0;
	float rowWidth = 0;
	float rowMinX = 0;
	float rowMaxX = 0;
	const char* rowStart = NULL;
	const char* rowEnd = NULL;
	const char* wordStart = NULL;
	float wordStartX = 0;
	float wordMinX = 0;
	const char* breakEnd = NULL;
	float breakWidth = 0;
	float breakMaxX = 0;
	int type = TTF_SPACE, ptype = TTF_SPACE;
	unsigned int pcodepoint = 0;

	if (maxRows == 0) {
		return 0;
	}

	if (end == NULL) {
		end = string + strlen(string);
	}

	if (string == end) {
		return 0;
	}

	fonsTextIterInit(ctx, &iter, 0, 0, string, end);
	prevIter = iter;
	while (fonsTextIterNext(ctx, &iter, &q)) {
		// FIXME: error checking
		/*
		if (iter.prevGlyphIndex < 0 && nvg__allocTextAtlas(ctx)) { // can not retrieve glyph?
			iter = prevIter;
			fonsTextIterNext(ctx, &iter, &q); // try again
		}
		*/
		prevIter = iter;
		switch (iter.codepoint) {
		case 9:			// \t
		case 11:		// \v
		case 12:		// \f
		case 32:		// space
		case 0x00a0:	// NBSP
			type = TTF_SPACE;
			break;
		case 10:		// \n
			type = pcodepoint == 13 ? TTF_SPACE : TTF_NEWLINE;
			break;
		case 13:		// \r
			type = pcodepoint == 10 ? TTF_SPACE : TTF_NEWLINE;
			break;
		case 0x0085:	// NEL
			type = TTF_NEWLINE;
			break;
		default:
			if ((iter.codepoint >= 0x4E00 && iter.codepoint <= 0x9FFF) ||
				(iter.codepoint >= 0x3000 && iter.codepoint <= 0x30FF) ||
				(iter.codepoint >= 0xFF00 && iter.codepoint <= 0xFFEF) ||
				(iter.codepoint >= 0x1100 && iter.codepoint <= 0x11FF) ||
				(iter.codepoint >= 0x3130 && iter.codepoint <= 0x318F) ||
				(iter.codepoint >= 0xAC00 && iter.codepoint <= 0xD7AF))
				type = TTF_CJK_CHAR;
			else
				type = TTF_CHAR;
			break;
		}

		if (type == TTF_NEWLINE) {
			// Always handle new lines.
			rows[nrows].start = rowStart != NULL ? rowStart : iter.str;
			rows[nrows].end = rowEnd != NULL ? rowEnd : iter.str;
			rows[nrows].width = rowWidth;
			rows[nrows].minx = rowMinX;
			rows[nrows].maxx = rowMaxX;
			rows[nrows].next = iter.next;
			nrows++;
			if (nrows >= maxRows) {
				return nrows;
			}
			// Set null break point
			breakEnd = rowStart;
			breakWidth = 0.0;
			breakMaxX = 0.0;
			// Indicate to skip the white space at the beginning of the row.
			rowStart = NULL;
			rowEnd = NULL;
			rowWidth = 0;
			rowMinX = rowMaxX = 0;
		}
		else {
			if (rowStart == NULL) {
				// Skip white space until the beginning of the line
				if (type == TTF_CHAR || type == TTF_CJK_CHAR) {
					// The current char is the row so far
					rowStartX = iter.x;
					rowStart = iter.str;
					rowEnd = iter.next;
					rowWidth = iter.nextx - rowStartX; // q.x1 - rowStartX;
					rowMinX = q.x0 - rowStartX;
					rowMaxX = q.x1 - rowStartX;
					wordStart = iter.str;
					wordStartX = iter.x;
					wordMinX = q.x0 - rowStartX;
					// Set null break point
					breakEnd = rowStart;
					breakWidth = 0.0;
					breakMaxX = 0.0;
				}
			}
			else {
				float nextWidth = iter.nextx - rowStartX;

				// track last non-white space character
				if (type == TTF_CHAR || type == TTF_CJK_CHAR) {
					rowEnd = iter.next;
					rowWidth = iter.nextx - rowStartX;
					rowMaxX = q.x1 - rowStartX;
				}
				// track last end of a word
				if (((ptype == TTF_CHAR || ptype == TTF_CJK_CHAR) && type == TTF_SPACE) || type == TTF_CJK_CHAR) {
					breakEnd = iter.str;
					breakWidth = rowWidth;
					breakMaxX = rowMaxX;
				}
				// track last beginning of a word
				if ((ptype == TTF_SPACE && (type == TTF_CHAR || type == TTF_CJK_CHAR)) || type == TTF_CJK_CHAR) {
					wordStart = iter.str;
					wordStartX = iter.x;
					wordMinX = q.x0 - rowStartX;
				}

				// Break to new line when a character is beyond break width.
				if ((type == TTF_CHAR || type == TTF_CJK_CHAR) && (nextWidth > breakRowWidth && breakRowWidth > 0)) {
					// The run length is too long, need to break to new line.
					if (breakEnd == rowStart) {
						// The current word is longer than the row length, just break it from here.
						rows[nrows].start = rowStart;
						rows[nrows].end = iter.str;
						rows[nrows].width = rowWidth;
						rows[nrows].minx = rowMinX;
						rows[nrows].maxx = rowMaxX;
						rows[nrows].next = iter.str;
						nrows++;
						if (nrows >= maxRows)
							return nrows;
						rowStartX = iter.x;
						rowStart = iter.str;
						rowEnd = iter.next;
						rowWidth = iter.nextx - rowStartX;
						rowMinX = q.x0 - rowStartX;
						rowMaxX = q.x1 - rowStartX;
						wordStart = iter.str;
						wordStartX = iter.x;
						wordMinX = q.x0 - rowStartX;
					}
					else {
						// Break the line from the end of the last word, and start new line from the beginning of the new.
						rows[nrows].start = rowStart;
						rows[nrows].end = breakEnd;
						rows[nrows].width = breakWidth;
						rows[nrows].minx = rowMinX;
						rows[nrows].maxx = breakMaxX;
						rows[nrows].next = wordStart;
						nrows++;
						if (nrows >= maxRows)
							return nrows;
						rowStartX = wordStartX;
						rowStart = wordStart;
						rowEnd = iter.next;
						rowWidth = iter.nextx - rowStartX;
						rowMinX = wordMinX;
						rowMaxX = q.x1 - rowStartX;
						// No change to the word start
					}
					// Set null break point
					breakEnd = rowStart;
					breakWidth = 0.0;
					breakMaxX = 0.0;
				}
			}
		}

		pcodepoint = iter.codepoint;
		ptype = type;
	}

	// Break the line from the end of the last word, and start new line from the beginning of the new.
	if (rowStart != NULL) {
		rows[nrows].start = rowStart;
		rows[nrows].end = rowEnd;
		rows[nrows].width = rowWidth;
		rows[nrows].minx = rowMinX;
		rows[nrows].maxx = rowMaxX;
		rows[nrows].next = end;
		nrows++;
	}

	return nrows;
}

void TTF_TextBox(const drawTextCommand_t *cmd, const char *string) {
	TTFtextRow rows[2];
	int nrows = 0, i;
	int oldAlign = state.align;
	int halign = state.align & (FONS_ALIGN_LEFT | FONS_ALIGN_CENTER | FONS_ALIGN_RIGHT);
	int valign = state.align & (FONS_ALIGN_TOP | FONS_ALIGN_MIDDLE | FONS_ALIGN_BOTTOM | FONS_ALIGN_BASELINE);
	float lineh; // FIXME: lineheight

	fonsVertMetrics(ctx, nullptr, nullptr, &lineh);
	fonsSetAlign(ctx, FONS_ALIGN_LEFT | valign);

	float x = cmd->x;
	float y = cmd->y;
	lineh *= state.lineHeight;

	while ((nrows = TTF_BreakLines(string, nullptr, cmd->w, rows, 2)) > 0) {
		for (i = 0; i < nrows; i++) {
			TTFtextRow* row = &rows[i];
			if (halign & FONS_ALIGN_LEFT)
				fonsDrawText(ctx, x, y, row->start, row->end);
			else if (halign & FONS_ALIGN_CENTER)
				fonsDrawText(ctx, x + cmd->w * 0.5f - row->width*0.5f, y, row->start, row->end);
			else if (halign & FONS_ALIGN_RIGHT)
				fonsDrawText(ctx, x + cmd->w - row->width, y, row->start, row->end);
			y += lineh;
		}
		string = rows[nrows - 1].next;
	}

	fonsSetAlign(ctx, oldAlign);

}

int Asset_TextWidth(AssetHandle assetHandle, const char *string, float scale) {
	Asset *asset = Asset_Get(ASSET_ANY, assetHandle);

	assert(asset != nullptr);

	if (asset->type != ASSET_BITMAPFONT && asset->type != ASSET_FONT) {
		Com_Error(ERR_DROP, "Asset_TextWidth: asset not font or bmpfont");
		return -1;
	}

	if (asset->type == ASSET_BITMAPFONT) {
		return BMPFNT_TextWidth(assetHandle, string, scale);
	} else {
		TTFFont_t *fnt = (TTFFont_t*)asset->resource;

		fonsSetFont(ctx, fnt->hnd);
		fonsSetSize(ctx, scale);
		return (int) fonsTextBounds(ctx, 0, 0, string, nullptr, nullptr);
	}
}