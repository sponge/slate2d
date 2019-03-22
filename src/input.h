#pragma once
#include <SDL/SDL_events.h>
#include "shared.h"

#define MAX_CONTROLLERS 4
#define CONTROLLER_KEY_START SDL_NUM_SCANCODES + SDL_BUTTON_X2 + 1
#define MAX_KEYS (CONTROLLER_KEY_START + (SDL_CONTROLLER_BUTTON_MAX * MAX_CONTROLLERS))

void ProcessInputEvent(SDL_Event ev);
bool In_ButtonPressed(int buttonId, unsigned int delay, int repeat);
MousePosition In_MousePosition();
int In_GetKeyNum(const char *str);
const char * In_GetKeyName(int key);
