#include <nanovg.h>
#include <map>
#include <string>
#include <stdlib.h>
#include "image.h"
#include "files.h"

std::map<std::string, Img *> imgs;

Img * Img_Create(const char *name, const char *path) {
	if (imgs[name] != nullptr ) {
		return imgs[name];
	}

	auto img = new Img();
	strncpy(img->path, path, sizeof(img->path));

	imgs[name] = img;
	return img;
}

void Img_Load(NVGcontext *nvg, Img &img) {
	if (img.hnd != 0) {
		nvgDeleteImage(img.nvg, img.hnd);
	}

	void *buffer;
	auto sz = FS_ReadFile(img.path, &buffer);

	img.nvg = nvg;
	img.hnd = nvgCreateImageMem(img.nvg, NVG_IMAGE_NEAREST, (unsigned char *)buffer, sz);
	nvgImageSize(img.nvg, img.hnd, &img.w, &img.h);

	free(buffer);
}

Img * Img_Find(const char *name) {
	return imgs[name];
}

void Img_LoadAll(NVGcontext *nvg) {
	for (auto img : imgs) {
		if (img.second->hnd == 0) {
			Img_Load(nvg, *(img.second));
		}
	}
}

bool Img_Free(const char *name) {
	auto img = imgs[name];

	if (img == nullptr) {
		return false;
	}

	nvgDeleteImage(img->nvg, img->hnd);
	imgs.erase(name);
	delete imgs[name];
	return true;
}

void Img_Clear() {
	for (auto img : imgs) {
		nvgDeleteImage(img.second->nvg, img.second->hnd);
	}
	imgs.clear();
}