#include <fontstash.h>
#include "assetloader.h"
#include "files.h"
#include "console/console.h"
#include "external/fontstash.h"
#include "external/gl3corefontstash.h"

FONScontext *ctx;

extern ClientInfo inf;

// can't unload TTFs, so we'll need to make sure we don't keep reloading them, sadly
void* TTF_Load(Asset &asset) {
	TTFFont_t *fnt = new TTFFont_t();

	if (ctx == nullptr) {
		ctx = glfonsCreate(512, 512, 0);
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
	assert(sz != -1);

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