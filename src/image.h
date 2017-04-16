#pragma once
#include <nanovg.h>

typedef struct {
	NVGcontext *nvg;
	int width, height;
} ClientInfo;

typedef struct {
	NVGcontext *nvg;
	unsigned int index;
	unsigned int hnd;
	int w, h;
	char name[64];
	char path[64];
} Img;

Img * Img_Create(const char * name, const char * path);
void Img_Load(Img & img);
void Img_LoadAll();
Img * Img_Find(const char * name);
Img * Img_Get(unsigned int i);
bool Img_Free(const char * name);
void Img_Clear();
