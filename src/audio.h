#pragma once
#include "../game/shared.h"

unsigned int Snd_Play(AssetHandle assetHandle, float volume, float pan, bool loop);
void Snd_Stop(unsigned int handle);
void Snd_PauseResume(unsigned int handle, bool pause);