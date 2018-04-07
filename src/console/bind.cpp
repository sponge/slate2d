/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
#include "console.h"
#include <SDL/SDL_keyboard.h>
#include <SDL/SDL_mouse.h>
#include <SDL/SDL_gamecontroller.h>

qkey_t		keys[SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1)];

typedef struct {
	const char	*name;
	int		keynum;
} keyname_t;

// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[] =
{
	{ "GRAVE", SDL_SCANCODE_GRAVE },
	{ "TAB", SDL_SCANCODE_TAB },
	{ "ENTER", SDL_SCANCODE_RETURN },
	{ "ESCAPE", SDL_SCANCODE_ESCAPE },
	{ "SPACE", SDL_SCANCODE_SPACE },
	{ "BACKSPACE", SDL_SCANCODE_BACKSPACE },
	{ "UPARROW", SDL_SCANCODE_UP },
	{ "DOWNARROW", SDL_SCANCODE_DOWN },
	{ "LEFTARROW", SDL_SCANCODE_LEFT },
	{ "RIGHTARROW", SDL_SCANCODE_RIGHT },

	{ "ALT", SDL_SCANCODE_LALT },
	{ "CTRL", SDL_SCANCODE_LCTRL },
	{ "SHIFT", SDL_SCANCODE_LSHIFT },
	{ "RALT", SDL_SCANCODE_RALT },
	{ "RCTRL", SDL_SCANCODE_RCTRL },
	{ "RSHIFT", SDL_SCANCODE_RSHIFT },

	{ "LGUI", SDL_SCANCODE_LGUI },
	{ "RGUI", SDL_SCANCODE_RGUI },

	{ "CAPSLOCK", SDL_SCANCODE_CAPSLOCK },

	{ "F1", SDL_SCANCODE_F1 },
	{ "F2", SDL_SCANCODE_F2 },
	{ "F3", SDL_SCANCODE_F3 },
	{ "F4", SDL_SCANCODE_F4 },
	{ "F5", SDL_SCANCODE_F5 },
	{ "F6", SDL_SCANCODE_F6 },
	{ "F7", SDL_SCANCODE_F7 },
	{ "F8", SDL_SCANCODE_F8 },
	{ "F9", SDL_SCANCODE_F9 },
	{ "F10", SDL_SCANCODE_F10 },
	{ "F11", SDL_SCANCODE_F11 },
	{ "F12", SDL_SCANCODE_F12 },

	{ "INS", SDL_SCANCODE_INSERT },
	{ "DEL", SDL_SCANCODE_DELETE },
	{ "PGDN", SDL_SCANCODE_PAGEDOWN },
	{ "PGUP", SDL_SCANCODE_PAGEUP },
	{ "HOME", SDL_SCANCODE_HOME },
	{ "END", SDL_SCANCODE_END },

	{ "MOUSE1", SDL_NUM_SCANCODES + MOUSE_BUTTON_LEFT },
	{ "MOUSE2", SDL_NUM_SCANCODES + MOUSE_BUTTON_RIGHT },
	{ "MOUSE3", SDL_NUM_SCANCODES + MOUSE_BUTTON_MIDDLE },
	{ "MOUSE4", SDL_NUM_SCANCODES + MOUSE_BUTTON_X1 },
	{ "MOUSE5", SDL_NUM_SCANCODES + MOUSE_BUTTON_X2 },

	{ "PAD1_A", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_A },
	{ "PAD1_B", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_B },
	{ "PAD1_X", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_X },
	{ "PAD1_Y", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_Y },
	{ "PAD1_BACK", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_BACK },
	{ "PAD1_GUIDE", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_GUIDE },
	{ "PAD1_START", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_START },
	{ "PAD1_L3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_LEFTSTICK },
	{ "PAD1_R3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_RIGHTSTICK },
	{ "PAD1_L1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
	{ "PAD1_R1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
	{ "PAD1_UP", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_DPAD_UP },
	{ "PAD1_DOWN", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_DPAD_DOWN },
	{ "PAD1_LEFT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_DPAD_LEFT },
	{ "PAD1_RIGHT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 0) + SDL_CONTROLLER_BUTTON_DPAD_RIGHT },

	{ "KP_1",			SDL_SCANCODE_KP_1 },
	{ "KP_2",			SDL_SCANCODE_KP_2 },
	{ "KP_3",			SDL_SCANCODE_KP_3 },
	{ "KP_4",			SDL_SCANCODE_KP_4 },
	{ "KP_5",			SDL_SCANCODE_KP_5 },
	{ "KP_6",			SDL_SCANCODE_KP_6 },
	{ "KP_7",			SDL_SCANCODE_KP_7 },
	{ "KP_8",			SDL_SCANCODE_KP_8 },
	{ "KP_9",			SDL_SCANCODE_KP_9 },
	{ "KP_0",			SDL_SCANCODE_KP_0 },
	{ "KP_PERIOD",		SDL_SCANCODE_KP_PERIOD },
	{ "KP_ENTER",		SDL_SCANCODE_KP_ENTER },
	{ "KP_SLASH",		SDL_SCANCODE_KP_DIVIDE },
	{ "KP_MINUS",		SDL_SCANCODE_KP_MINUS },
	{ "KP_PLUS",		SDL_SCANCODE_KP_PLUS },
	{ "KP_STAR",		SDL_SCANCODE_KP_MULTIPLY },
	{ "KP_EQUALS",		SDL_SCANCODE_KP_EQUALS },

	{ "PAUSE", SDL_SCANCODE_PAUSE },

	{ "SEMICOLON", ';' },	// because a raw semicolon seperates commands

	{ NULL,0 }
};

/*
===================
Key_StringToKeynum

Returns a key number to be used to index keys[] by looking at
the given string.  Single ascii characters return themselves, while
the SDL_SCANCODE_* names are matched up.

0x11 will be interpreted as raw hex, which will allow new controlers

to be configured even if they don't have defined names.
===================
*/
int Key_StringToKeynum(const char *str) {

	keyname_t	*kn;

	if (!str || !str[0]) {
		return -1;
	}
	if (!str[1]) {
		if (str[0] >= '1' && str[0] <= '9') {
			return str[0] - 19;
		}
		else if (str[0] == '0') {
			return SDL_SCANCODE_0;
		}
		return toupper(str[0]) - 61;
	}

	// check for hex code
	if (str[0] == '0' && str[1] == 'x' && strlen(str) == 4) {
		int		n1, n2;

		n1 = str[2];
		if (n1 >= '0' && n1 <= '9') {
			n1 -= '0';
		}
		else if (n1 >= 'a' && n1 <= 'f') {
			n1 = n1 - 'a' + 10;
		}
		else {
			n1 = 0;
		}

		n2 = str[3];
		if (n2 >= '0' && n2 <= '9') {
			n2 -= '0';
		}
		else if (n2 >= 'a' && n2 <= 'f') {
			n2 = n2 - 'a' + 10;
		}
		else {
			n2 = 0;
		}

		return n1 * 16 + n2;
	}

	// scan for a text match
	for (kn = keynames; kn->name; kn++) {
		if (!strcasecmp(str, kn->name))
			return kn->keynum;
	}

	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, a SDL_SCANCODE_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
const char *Key_KeynumToString(int keynum) {
	keyname_t	*kn;
	static	char	tinystr[5];
	int			i, j;

	if (keynum == -1) {
		return "<KEY NOT FOUND>";
	}

	if (keynum < 0 || keynum >= SDL_NUM_SCANCODES) {
		return "<OUT OF RANGE>";
	}

	if (keynum >= SDL_SCANCODE_A && keynum <= SDL_SCANCODE_Z) {
		tinystr[0] = keynum + 61;
		tinystr[1] = 0;
		return tinystr;
	}

	if (keynum >= SDL_SCANCODE_1 && keynum <= SDL_SCANCODE_9) {
		tinystr[0] = keynum + 18;
		tinystr[1] = 0;
		return tinystr;
	}
	else if (keynum == SDL_SCANCODE_0) {
		tinystr[0] = '0';
		tinystr[1] = 0;
		return tinystr;
	}

	// check for a key string
	for (kn = keynames; kn->name; kn++) {
		if (keynum == kn->keynum) {
			return kn->name;
		}
	}

	// make a hex string
	i = keynum >> 4;
	j = keynum & 15;

	tinystr[0] = '0';
	tinystr[1] = 'x';
	tinystr[2] = i > 9 ? i - 10 + 'a' : i + '0';
	tinystr[3] = j > 9 ? j - 10 + 'a' : j + '0';
	tinystr[4] = 0;

	return tinystr;
}

/*
===================
Key_SetBinding
===================
*/
void Key_SetBinding(int keynum, const char *binding) {
	if (keynum == -1) {
		return;
	}

	// free old bindings
	if (keys[keynum].binding) {
		free(keys[keynum].binding);
	}

	// allocate memory for new binding
	keys[keynum].binding = CopyString(binding);

	// consider this like modifying an archived cvar, so the
	// file write will be triggered at the next oportunity
	// FIXME
	//cvar_modifiedFlags |= CVAR_ARCHIVE;
}

/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f(void)
{
	int			i, c, b;
	char		cmd[1024];

	c = Cmd_Argc();

	if (c < 2)
	{
		Com_Printf("bind <key> [command] : attach a command to a key\n");
		return;
	}
	b = Key_StringToKeynum(Cmd_Argv(1));
	if (b == -1)
	{
		Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2)
	{
		if (keys[b].binding)
			Com_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), keys[b].binding);
		else
			Com_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
		return;
	}

	// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i = 2; i< c; i++)
	{
		strcat(cmd, Cmd_Argv(i));
		if (i != (c - 1))
			strcat(cmd, " ");
	}

	Key_SetBinding(b, cmd);
}


/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f(void)
{
	int		b;

	if (Cmd_Argc() != 2)
	{
		Com_Printf("unbind <key> : remove commands from a key\n");
		return;
	}

	b = Key_StringToKeynum(Cmd_Argv(1));
	if (b == -1)
	{
		Com_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding(b, "");
}

/*
===================
Key_Unbindall_f
===================
*/
void Key_Unbindall_f(void)
{
	int		i;

	for (i = 0; i<256; i++)
		if (keys[i].binding)
			Key_SetBinding(i, "");
}

/*
============
Key_Bindlist_f
============
*/
void Key_Bindlist_f(void) {
	int		i;

	for (i = 0; i < 256; i++) {
		if (keys[i].binding && keys[i].binding[0]) {
			Com_Printf("%s \"%s\"\n", Key_KeynumToString(i), keys[i].binding);
		}
	}
}


/*
===================
CL_InitKeyCommands
===================
*/
void CL_InitKeyCommands(void) {
	// register our functions
	Cmd_AddCommand("bind", Key_Bind_f);
	Cmd_AddCommand("unbind", Key_Unbind_f);
	Cmd_AddCommand("unbindall", Key_Unbindall_f);
	Cmd_AddCommand("bindlist", Key_Bindlist_f);
}