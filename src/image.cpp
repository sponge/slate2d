#include <nanovg.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include "image.h"
#include "files.h"
#include "console/console.h"

// FIXME: work out how to do this with a vector of structs instead vector of ptrs
std::vector<Img *> imgs;
extern ClientInfo inf;

Img * Img_Create(const char *name, const char *path) {
	Com_Printf("img_create: %s %s\n", name, path);

	auto found = Img_Find(name);
	if (found != nullptr) {
		return found;
	}

	auto img = new Img();

	img->index = imgs.size();
	strncpy(img->path, path, sizeof(img->path));
	strncpy(img->name, name, sizeof(img->name));

	imgs.push_back(img);
	return img;
}

void Img_Load(Img &img) {
	Com_Printf("img_load: %s %s\n", img.name, img.path);
	if (img.hnd != 0) {
		nvgDeleteImage(img.nvg, img.hnd);
	}

	void *buffer;
	auto sz = FS_ReadFile(img.path, &buffer);

	img.nvg = inf.nvg;
	img.hnd = nvgCreateImageMem(img.nvg, NVG_IMAGE_NEAREST, (unsigned char *)buffer, sz);
	nvgImageSize(img.nvg, img.hnd, &img.w, &img.h);

	free(buffer);
}

Img * Img_Get(unsigned int i) {
	return imgs[i];
}

Img * Img_Find(const char *name) {
	for (auto img : imgs) {
		if (strcmp(img->name, name) == 0) {
			return img;
		}
	}

	return nullptr;
}

void Img_LoadAll() {
	for (auto img : imgs) {
		if (img->hnd == 0) {
			Img_Load(*img);
		}
	}
}

bool Img_Free(const char *name) {
	Com_Printf("img_free: trying to free %s... ", name);
	for (auto img : imgs) {
		if (strcmp(img->name, name) == 0) {
			nvgDeleteImage(img->nvg, img->hnd);
			img->hnd = 0;
			img->nvg = nullptr;
			Com_Printf("found!\n");
			return true;
		}
	}

	Com_Printf("not found!\n");
	return false;
}

void Img_Clear() {
	for (auto img : imgs) {
		nvgDeleteImage(img->nvg, img->hnd);
	}
	imgs.clear();
}