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

Img * Img_Create(const char * name, const char * path);
void Img_Load(NVGcontext * nvg, Img & img);
void Img_LoadAll(NVGcontext * nvg);
Img * Img_Find(const char * name);
bool Img_Free(const char * name);
void Img_Clear();
