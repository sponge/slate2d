#include <SDL/SDL_keyboard.h>
#include <SDL/SDL_mouse.h>
#include <SDL/SDL_gamecontroller.h>

#include "console/console.h"
#include "input.h"
#include "main.h"

/*
===================
CL_AddKeyUpCommands
===================
*/
void CL_AddKeyUpCommands(int key, char *kb, unsigned time) {
	int i;
	char button[1024], *buttonPtr;
	char	cmd[1024];
	bool keyevent;

	if (!kb) {
		return;
	}
	keyevent = false;
	buttonPtr = button;
	for (i = 0; ; i++) {
		if (kb[i] == ';' || !kb[i]) {
			*buttonPtr = '\0';
			if (button[0] == '+') {
				// button commands add keynum and time as parms so that multiple
				// sources can be discriminated and subframe corrected
				Com_sprintf(cmd, sizeof(cmd), "-%s %i %i\n", button + 1, key, time);
				Cbuf_AddText(cmd);
				keyevent = true;
			}
			else {
				if (keyevent) {
					// down-only command
					Cbuf_AddText(button);
					Cbuf_AddText("\n");
				}
			}
			buttonPtr = button;
			while ((kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0) {
				i++;
			}
		}
		*buttonPtr++ = kb[i];
		if (!kb[i]) {
			break;
		}
	}
}

const char *IN_BindForKey(int key) {
	return keys[key].binding == nullptr ? "" : keys[key].binding;
}

bool KeyEvent(int key, bool down, unsigned time) {
	// send the bound action
	auto kb = keys[key].binding;
	char	cmd[1024];

	if (key > sizeof(keys) / sizeof(keys[0])) {
		Com_Printf("key number %i is out of bounds!\n", key);
		return false;
	}

	if (!kb) {
		return false;
	}

	if (!down) {
		CL_AddKeyUpCommands(key, kb, time);
		return true;
	}
	
	if (kb[0] == '+') {
		int i;
		char button[1024], *buttonPtr;
		buttonPtr = button;
		for (i = 0; ; i++) {
			if (kb[i] == ';' || !kb[i]) {
				*buttonPtr = '\0';
				if (button[0] == '+') {
					// button commands add keynum and time as parms so that multiple
					// sources can be discriminated and subframe corrected
					Com_sprintf(cmd, sizeof(cmd), "%s %i %i\n", button, key, time);
					Cbuf_AddText(cmd);
				}
				else {
					// down-only command
					Cbuf_AddText(button);
					Cbuf_AddText("\n");
				}
				buttonPtr = button;
				while ((kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0) {
					i++;
				}
			}
			*buttonPtr++ = kb[i];
			if (!kb[i]) {
				break;
			}
		}
		return true;
	}
	else {
		// down-only command
		Cbuf_AddText(kb);
		Cbuf_AddText("\n");
		return true;
	}
}

bool MouseEvent(int button, bool down, unsigned time) {
	switch (button) {
	case SDL_BUTTON_LEFT:
		return KeyEvent(SDL_NUM_SCANCODES + MOUSE_BUTTON_LEFT, down, com_frameTime);
	case SDL_BUTTON_RIGHT:
		return KeyEvent(SDL_NUM_SCANCODES + MOUSE_BUTTON_RIGHT, down, com_frameTime);
	case SDL_BUTTON_MIDDLE:
		return KeyEvent(SDL_NUM_SCANCODES + MOUSE_BUTTON_MIDDLE, down, com_frameTime);
	case SDL_BUTTON_X1:
		return KeyEvent(SDL_NUM_SCANCODES + MOUSE_BUTTON_X1, down, com_frameTime);
	case SDL_BUTTON_X2:
		return KeyEvent(SDL_NUM_SCANCODES + MOUSE_BUTTON_X2, down, com_frameTime);
	}

	return false;
}

bool JoyEvent(int controller, int button, bool down, unsigned time) {
	if (controller >= MAX_CONTROLLERS) {
		Com_Printf("ignoring controller %i > MAX_CONTROLLERS\n", controller);
		return false;
	}

	int keyId = SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (controller * SDL_CONTROLLER_BUTTON_MAX) + button;

	return KeyEvent(keyId, down, time);
}

void IN_KeyDown(kbutton_t *b) {
	int		k;
	const char	*c;

	c = Cmd_Argv(1);
	if (c[0]) {
		k = atoi(c);
	}
	else {
		k = -1;		// typed manually at the console for continuous down
	}

	if (k == b->down[0] || k == b->down[1]) {
		return;		// repeating key
	}

	if (!b->down[0]) {
		b->down[0] = k;
	}
	else if (!b->down[1]) {
		b->down[1] = k;
	}
	else {
		Com_Printf("Three keys down for a button!\n");
		return;
	}

	if (b->active) {
		return;		// still down
	}

	// save timestamp for partial frame summing
	c = Cmd_Argv(2);
	b->downtime = atoi(c);
	b->firstdowntime = b->downtime;

	b->active = true;
	b->wasPressed = true;
}

void IN_KeyUp(kbutton_t *b) {
	int		k;
	const char	*c;
	unsigned	uptime;

	c = Cmd_Argv(1);
	if (c[0]) {
		k = atoi(c);
	}
	else {
		// typed manually at the console, assume for unsticking, so clear all
		b->down[0] = b->down[1] = 0;
		b->active = false;
		return;
	}

	if (b->down[0] == k) {
		b->down[0] = 0;
	}
	else if (b->down[1] == k) {
		b->down[1] = 0;
	}
	else {
		return;		// key up without coresponding down (menu pass through)
	}
	if (b->down[0] || b->down[1]) {
		return;		// some other key is still holding it down
	}

	b->active = false;

	// save timestamp for partial frame summing
	c = Cmd_Argv(2);
	uptime = atoi(c);
	if (uptime) {
		b->msec += uptime - b->downtime;
	}
	else {
		b->msec += frame_msec / 2;
	}

	b->active = false;
}

/*
===============
IN_KeyState

Returns the fraction of the frame that the key was down
===============
*/
float IN_KeyState(kbutton_t *key) {
	// q3 would do this fraction stuff but i'm not sure if it even works anymore
	// just return if it's down or not since it was breaking slomo

	return key->active ? 1.0f : 0.0f;

#if 0
	float		val;
	double		msec;

	msec = key->msec;
	key->msec = 0;

	if (key->active) {
		// still down
		if (!key->downtime) {
			msec = com_frameTime;
		}
		else {
			msec += com_frameTime - key->downtime;
		}
		key->downtime = com_frameTime;
	}

	val = (float)msec / frame_msec;

	if (val < 0) {
		val = 0;
	}
	if (val > 1) {
		val = 1;
	}

	return val;
#endif
}

bool IN_KeyPressed(kbutton_t *key, unsigned int delay, unsigned int repeat) {
	if (key->active == false) {
		return false;
	}

	double firstTrigger = key->firstdowntime + delay;

	if (com_frameTime >= firstTrigger && com_frameTime - frame_msec < firstTrigger) {
		return true;
	}

	double heldTime = com_frameTime - key->firstdowntime - delay;

	if (heldTime < 0) {
		return false;
	}

	if (repeat < 0) {
		return com_frameTime == key->firstdowntime - delay;
	}

	double repeatCount = SDL_floor(heldTime / repeat);
	double lastRepeatCount = SDL_floor((heldTime - frame_msec) / repeat);
	//Com_Printf("current:%0.5f last:%0.5f", heldTime / repeat, (heldTime - frame_msec) / repeat);
	//Com_Printf(lastRepeatCount != repeatCount ? " TRIGGER\n" : "\n");

	return lastRepeatCount != repeatCount;
}

MousePosition IN_MousePosition() {
	MousePosition mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	return mousePos;
}