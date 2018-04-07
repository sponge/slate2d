#pragma once
#include "../game/shared.h"

bool KeyEvent(int key, bool down, unsigned time);
bool MouseEvent(int button, bool down, unsigned time);
bool JoyEvent(int controller, int button, bool down, unsigned time);
void IN_KeyDown(kbutton_t *b);
void IN_KeyUp(kbutton_t *b);
float IN_KeyState(kbutton_t *key);
bool IN_KeyPressed(kbutton_t *key, unsigned int delay, unsigned int repeat);
MousePosition IN_MousePosition();