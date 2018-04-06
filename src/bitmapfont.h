#pragma once
#include "assetloader.h"

int BMPFNT_TextWidth(AssetHandle assetHandle, const char *string, float scale);
int BMPFNT_DrawText(AssetHandle assetHandle, float x, float y, float scale, const char *string);