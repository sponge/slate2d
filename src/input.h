#pragma once
#include <SDL/SDL_events.h>
#include "../game/shared.h"

#define NUM_MOUSE_BUTTONS SDL_BUTTON_X2
#define MAX_CONTROLLERS 4
#define MAX_KEYS (SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * MAX_CONTROLLERS))

void ProcessInputEvent(SDL_Event ev);
bool In_ButtonPressed(int buttonId, unsigned int delay, int repeat);
MousePosition In_MousePosition();
int Key_StringToKeynum(const char *str);
const char *Key_KeynumToString(int keynum);
