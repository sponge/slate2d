#include "cvar_main.h"

typedef struct {
	conVar_t   		**cvar;
	const char		*cvarName;
	const char		*defaultString;
	int	     		cvarFlags;
} conVarTable_t;

conVar_t *eng_errorMessage;
conVar_t *eng_lastErrorStack;
conVar_t *vid_width;
conVar_t *vid_height;
conVar_t *vid_swapinterval;
conVar_t *vid_fullscreen;
conVar_t *vid_showfps;
conVar_t *vid_maxfps;
conVar_t *eng_pause;
conVar_t *snd_volume;
conVar_t *debug_fontAtlas;
conVar_t *debug_assets;
conVar_t *debug_imguidemo;

static conVarTable_t mainCvarTable[] = {
    { &eng_errorMessage, "engine.errorMessage", "", 0 },
	{ &eng_lastErrorStack, "engine.lastErrorStack", "", 0 },
    { &vid_width, "vid.width", "1280", 0 },
    { &vid_height, "vid.height", "720", 0 },
    { &vid_swapinterval, "vid.swapInterval", "1", 0 },
    { &vid_fullscreen, "vid.fullscreen", "0", 0 },
    { &vid_showfps, "vid.showfps", "0", 0 },
	{ &vid_maxfps, "vid.maxfps", "120", 0 },
	{ &eng_pause, "engine.pause", "0", 0 },
	{ &snd_volume, "snd.volume", "1.0", 0 },
	{ &debug_fontAtlas, "debug.fontAtlas", "0", 0 },
	{ &debug_assets, "debug.assets", "0", 0 },
	{ &debug_imguidemo, "debug.imguiDemo", "0", 0 },
	{ NULL }
};

void RegisterMainCvars( void ) {
    conVarTable_t *cv = &mainCvarTable[0];
	while (cv->cvar != NULL) {
        *cv->cvar = Con_GetVarDefault(cv->cvarName, cv->defaultString, cv->cvarFlags);
		cv++;
    }
}