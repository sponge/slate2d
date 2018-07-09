#include <nanovg.h>
#include <fontstash.h>
#include "assetloader.h"
#include "files.h"
#include "console/console.h"

extern ClientInfo inf;

// can't unload TTFs, so we'll need to make sure we don't keep reloading them, sadly
void* TTF_Load(Asset &asset) {
	int found = nvgFindFont(inf.nvg, asset.name);
	if (found != -1) {
		return (void*)found;
	}
	unsigned char *font;
	auto sz = FS_ReadFile(asset.path, (void **)&font);
	assert(sz != -1);
	int hnd = nvgCreateFontMem(inf.nvg, asset.name, font, sz, 1);
	if (hnd < 0) {
		return nullptr;
	}
	free(font);

	return (void*)hnd;
}
void TTF_Free(Asset &asset) {
	// not supported
}