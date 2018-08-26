#pragma once
#include "../game/shared.h"

void SubmitRenderCommands(renderCommandList_t *list);
void DrawImage(float x, float y, float w, float h, float ox, float oy, float alpha, float scale, byte flipBits, Image *img, unsigned int shaderId);