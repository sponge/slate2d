#pragma once
#include <nanovg.h>

#include "../game/shared.h"

Img * Img_Create(const char * name, const char * path);
void Img_Load(Img & img);
void Img_LoadAll();
Img * Img_Find(const char * name);
Img * Img_Get(unsigned int i);
bool Img_Free(const char * name);
void Img_Clear();
