#pragma once
#include <SDL/SDL.h>
#include <nanovg.h>

typedef struct {
	SDL_Window *window;
	NVGcontext *nvg;
	int width, height;
} ClientInfo;

typedef struct {
	NVGcontext *nvg;
	int hnd;
	int w, h;
	char path[64];
} Img;

Img * Img_Load(const NVGcontext *nvg, char *path);