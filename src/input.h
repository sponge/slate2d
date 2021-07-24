#pragma once
#include <SDL/SDL_events.h>
#include "slate2d.h"

void ProcessInputEvent(SDL_Event ev);
bool In_ButtonPressed(int buttonId, unsigned int delay, int repeat);
MousePosition In_MousePosition();
AnalogAxes In_ControllerAnalog(int controllerNum);
int In_GetKeyNum(const char *str);
const char * In_GetKeyName(int key);
