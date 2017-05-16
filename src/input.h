#pragma once
#include "../game/shared.h"

bool KeyEvent(int key, bool down, unsigned time);
void IN_KeyDown(kbutton_t *b);
void IN_KeyUp(kbutton_t *b);
float CL_KeyState(kbutton_t *key);