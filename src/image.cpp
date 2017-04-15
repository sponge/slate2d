#include <nanovg.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include "image.h"
#include "files.h"

// FIXME: work out how to do this with a vector of structs instead vector of ptrs
std::vector<Img *> imgs;

Img * Img_Create(const char *name, const char *path) {
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

void Img_LoadAll(NVGcontext *nvg) {
	for (auto img : imgs) {
		if (img->hnd == 0) {
			Img_Load(nvg, *img);
		}
	}
}

bool Img_Free(const char *name) {
	auto index = 0;
	for (auto img : imgs) {
		if (strcmp(img->name, name) == 0) {
			nvgDeleteImage(img->nvg, img->hnd);
			imgs.erase(imgs.begin() + index);
			delete img;
			return true;

		}

		return true;
	}

	return false;
}

void Img_Clear() {
	for (auto img : imgs) {
		nvgDeleteImage(img->nvg, img->hnd);
	}
	imgs.clear();
}