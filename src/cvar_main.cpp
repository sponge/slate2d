#include "cvar_main.h"

cvar_t *com_errorMessage;
cvar_t *com_lastErrorStack;
cvar_t *vid_width;
cvar_t *vid_height;
cvar_t *vid_swapinterval;
cvar_t *vid_fullscreen;
cvar_t *vid_showfps;
cvar_t *com_sleepShortFrame;
cvar_t *com_pause;

static cvarTable_t mainCvarTable[] = {
    { &com_errorMessage, "com_errorMessage", "", CVAR_ROM },
	{ &com_lastErrorStack, "com_lastErrorStack", "", CVAR_ROM },
    { &vid_width, "vid_width", "1280", 0 },
    { &vid_height, "vid_height", "720", 0 },
    { &vid_swapinterval, "vid_swapInterval", "0", 0 },
    { &vid_fullscreen, "vid_fullscreen", "0", 0 },
    { &vid_showfps, "vid_showfps", "1", 0 },
	{ &com_sleepShortFrame, "com_sleepShortFrame", "1", 0 },
	{ &com_pause, "com_pause", "0", 0 },

};

static int mainCvarTableSize = sizeof( mainCvarTable ) / sizeof( mainCvarTable[0] );

void RegisterMainCvars( void ) {
    cvarTable_t *cv;
    int i;
	for ( i = 0, cv = mainCvarTable ; i < mainCvarTableSize ; i++, cv++ ) {
        *cv->cvar = Cvar_Get(cv->cvarName, cv->defaultString, cv->cvarFlags);
    }
}