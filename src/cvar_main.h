#pragma once
#include "console.h"

void RegisterMainCvars(void);

extern conVar_t *eng_errorMessage;
extern conVar_t *eng_lastErrorStack;
extern conVar_t *vid_maxfps;
extern conVar_t *vid_width;
extern conVar_t *vid_height;
extern conVar_t *vid_swapinterval;
extern conVar_t *vid_fullscreen;
extern conVar_t *vid_showfps;
extern conVar_t *eng_pause;
extern conVar_t *snd_volume;
extern conVar_t *debug_fontAtlas;
extern conVar_t *debug_assets;
extern conVar_t *debug_imguidemo;
extern conVar_t *debug_frameAdvanceStep;
extern conVar_t *in_deadzone;