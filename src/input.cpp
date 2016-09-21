#include "console\console.h"
#include "local.h"

kbutton_t in_1_left, in_1_right, in_1_up, in_1_down, in_1_jump, in_1_attack, in_1_menu;

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

bool KeyEvent(int key, bool down, unsigned time) {
	// send the bound action
	auto kb = keys[key].binding;
	char	cmd[1024];

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

	return false;
}

void IN_KeyDown(kbutton_t *b) {
	int		k;
	char	*c;

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

	b->active = true;
	b->wasPressed = true;
}

void IN_KeyUp(kbutton_t *b) {
	int		k;
	char	*c;
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
CL_KeyState

Returns the fraction of the frame that the key was down
===============
*/
float CL_KeyState(kbutton_t *key) {
	float		val;
	int			msec;

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

#if 0
	if (msec) {
		Com_Printf("%i ", msec);
	}
#endif

	val = (float)msec / frame_msec;
	if (val < 0) {
		val = 0;
	}
	if (val > 1) {
		val = 1;
	}

	return val;
}

void IN_P1_UpDown(void) { IN_KeyDown(&in_1_up); }
void IN_P1_UpUp(void) { IN_KeyUp(&in_1_up); }
void IN_P1_DownDown(void) { IN_KeyDown(&in_1_down); }
void IN_P1_DownUp(void) { IN_KeyUp(&in_1_down); }
void IN_P1_LeftDown(void) { IN_KeyDown(&in_1_left); }
void IN_P1_LeftUp(void) { IN_KeyUp(&in_1_left); }
void IN_P1_RightDown(void) { IN_KeyDown(&in_1_right); }
void IN_P1_RightUp(void) { IN_KeyUp(&in_1_right); }
void IN_P1_JumpDown(void) { IN_KeyDown(&in_1_jump); }
void IN_P1_JumpUp(void) { IN_KeyUp(&in_1_jump); }
void IN_P1_AttackDown(void) { IN_KeyDown(&in_1_attack); }
void IN_P1_AttackUp(void) { IN_KeyUp(&in_1_attack); }
void IN_P1_MenuDown(void) { IN_KeyDown(&in_1_menu); }
void IN_P1_MenuUp(void) { IN_KeyUp(&in_1_menu); }

void CL_InitInput(void) {
	Cmd_AddCommand("+p1up", IN_P1_UpDown);
	Cmd_AddCommand("-p1up", IN_P1_UpUp);
	Cmd_AddCommand("+p1down", IN_P1_DownDown);
	Cmd_AddCommand("-p1down", IN_P1_DownUp);
	Cmd_AddCommand("+p1left", IN_P1_LeftDown);
	Cmd_AddCommand("-p1left", IN_P1_LeftUp);
	Cmd_AddCommand("+p1right", IN_P1_RightDown);
	Cmd_AddCommand("-p1right", IN_P1_RightUp);
	Cmd_AddCommand("+p1jump", IN_P1_JumpDown);
	Cmd_AddCommand("-p1jump", IN_P1_JumpUp);
	Cmd_AddCommand("+p1attack", IN_P1_AttackDown);
	Cmd_AddCommand("-p1attack", IN_P1_AttackUp);
	Cmd_AddCommand("+p1menu", IN_P1_MenuDown);
	Cmd_AddCommand("-p1menu", IN_P1_MenuUp);
}

