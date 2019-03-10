#include <SDL/SDL.h>


#include "console/console.h"
#include "input.h"
#include "main.h"
#include <cmath>
#include <cstdint>

static inline void JoyEvent(int controller, int button, bool down, int64_t time) {
	if (controller >= MAX_CONTROLLERS) {
		Con_Printf("ignoring controller %i > MAX_CONTROLLERS\n", controller);
	}

	int keyId = SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (controller * SDL_CONTROLLER_BUTTON_MAX) + button;

	Con_HandleKeyPress(keyId, down, time);
}

void ProcessInputEvent(SDL_Event ev) {
	ImGuiIO &io = ImGui::GetIO();

	switch (ev.type) {
	case SDL_KEYUP:
		Con_HandleKeyPress(ev.key.keysym.scancode, false, com_frameTime);
		break;

	case SDL_KEYDOWN:
		if (io.WantCaptureKeyboard) {
			break;
		}

		Con_HandleKeyPress(ev.key.keysym.scancode, true, com_frameTime);
		break;

	case SDL_CONTROLLERDEVICEADDED: {
		if (ev.cdevice.which > MAX_CONTROLLERS) {
			break;
		}

		SDL_GameController *controller = SDL_GameControllerOpen(ev.cdevice.which);
		Con_Printf("Using controller at device index %i: %s\n", ev.cdevice.which, SDL_GameControllerName(controller));
		break;
	}

	case SDL_CONTROLLERDEVICEREMOVED: {
		SDL_GameController* controller = SDL_GameControllerFromInstanceID(ev.cdevice.which);
		Con_Printf("Closing controller instance %i: %s\n", ev.cdevice.which, SDL_GameControllerName(controller));
		SDL_GameControllerClose(controller);
		break;
	}

	case SDL_MOUSEBUTTONUP:
		Con_HandleKeyPress(SDL_NUM_SCANCODES + ev.button.button, false, com_frameTime);
		break;

	case SDL_MOUSEBUTTONDOWN:
		if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
			break;
		}

		Con_HandleKeyPress(ev.button.button, true, com_frameTime);
		break;

	case SDL_CONTROLLERBUTTONDOWN:
		JoyEvent(ev.jbutton.which, ev.jbutton.button, true, com_frameTime);
		break;

	case SDL_CONTROLLERBUTTONUP:
		JoyEvent(ev.jbutton.which, ev.jbutton.button, false, com_frameTime);
		break;
	}
}

bool IN_ButtonPressed(int buttonId, unsigned int delay, int repeat) {
	buttonState_t *button = Con_GetButton(buttonId);

	if (button == NULL || button->held == false) {
		return false;
	}

	int64_t muDelay = (int64_t) delay * 1000;
	int64_t muRepeat = (int64_t) repeat * 1000;

	int64_t firstTrigger = button->timestamp + muDelay;

	//Con_Printf("%i ft:%0.5f >= firstTrigger:%0.5f && lastft:%0.5f < firstTrigger:%0.5f - dt:%0.5f",
	//	com_frameTime >= firstTrigger && com_lastFrameTime < firstTrigger,
	//	com_frameTime,
	//	firstTrigger,
	//	com_lastFrameTime,
	//	firstTrigger,
	//	key->downtime
	//);

	if (com_frameTime >= firstTrigger && com_frameTime - frame_musec < firstTrigger) {
		return true;
	}

	int64_t heldTime = com_frameTime - button->timestamp - muDelay;

	if (heldTime < 0) {
		return false;
	}

	if (muRepeat == 0) {
		return true;
	}

	int64_t repeatCount = heldTime / muRepeat;
	int64_t lastRepeatCount = (heldTime - frame_musec) / muRepeat;
	//Con_Printf("current:%0.5f last:%0.5f", heldTime / muRepeat, (heldTime - frame_msec) / muRepeat);
	//Com_Printf(lastRepeatCount != repeatCount ? " TRIGGER\n" : "\n");

	return lastRepeatCount != repeatCount;
}

MousePosition IN_MousePosition() {
	MousePosition mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	return mousePos;
}

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

	{ "MOUSE1", SDL_NUM_SCANCODES + SDL_BUTTON_LEFT },
	{ "MOUSE2", SDL_NUM_SCANCODES + SDL_BUTTON_RIGHT },
	{ "MOUSE3", SDL_NUM_SCANCODES + SDL_BUTTON_MIDDLE },
	{ "MOUSE4", SDL_NUM_SCANCODES + SDL_BUTTON_X1 },
	{ "MOUSE5", SDL_NUM_SCANCODES + SDL_BUTTON_X2 },

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

	{ "PAD2_A", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_A },
	{ "PAD2_B", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_B },
	{ "PAD2_X", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_X },
	{ "PAD2_Y", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_Y },
	{ "PAD2_BACK", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_BACK },
	{ "PAD2_GUIDE", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_GUIDE },
	{ "PAD2_START", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_START },
	{ "PAD2_L3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_LEFTSTICK },
	{ "PAD2_R3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_RIGHTSTICK },
	{ "PAD2_L1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
	{ "PAD2_R1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
	{ "PAD2_UP", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_DPAD_UP },
	{ "PAD2_DOWN", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_DPAD_DOWN },
	{ "PAD2_LEFT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_DPAD_LEFT },
	{ "PAD2_RIGHT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 1) + SDL_CONTROLLER_BUTTON_DPAD_RIGHT },

	{ "PAD3_A", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_A },
	{ "PAD3_B", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_B },
	{ "PAD3_X", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_X },
	{ "PAD3_Y", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_Y },
	{ "PAD3_BACK", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_BACK },
	{ "PAD3_GUIDE", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_GUIDE },
	{ "PAD3_START", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_START },
	{ "PAD3_L3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_LEFTSTICK },
	{ "PAD3_R3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_RIGHTSTICK },
	{ "PAD3_L1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
	{ "PAD3_R1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
	{ "PAD3_UP", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_DPAD_UP },
	{ "PAD3_DOWN", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_DPAD_DOWN },
	{ "PAD3_LEFT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_DPAD_LEFT },
	{ "PAD3_RIGHT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 2) + SDL_CONTROLLER_BUTTON_DPAD_RIGHT },

	{ "PAD4_A", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_A },
	{ "PAD4_B", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_B },
	{ "PAD4_X", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_X },
	{ "PAD4_Y", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_Y },
	{ "PAD4_BACK", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_BACK },
	{ "PAD4_GUIDE", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_GUIDE },
	{ "PAD4_START", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_START },
	{ "PAD4_L3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_LEFTSTICK },
	{ "PAD4_R3", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_RIGHTSTICK },
	{ "PAD4_L1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_LEFTSHOULDER },
	{ "PAD4_R1", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_RIGHTSHOULDER },
	{ "PAD4_UP", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_DPAD_UP },
	{ "PAD4_DOWN", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_DPAD_DOWN },
	{ "PAD4_LEFT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_DPAD_LEFT },
	{ "PAD4_RIGHT", SDL_NUM_SCANCODES + NUM_MOUSE_BUTTONS + (SDL_CONTROLLER_BUTTON_MAX * 3) + SDL_CONTROLLER_BUTTON_DPAD_RIGHT },

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

	{ "SEMICOLON", SDL_SCANCODE_SEMICOLON },	// because a raw semicolon seperates commands

	{ NULL,0 }
};

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
		else if (str[0] >= 'a' && str[0] <= 'z') {
			return toupper(str[0]) - 61;
		}
		else {
			return SDL_GetScancodeFromName(str);
		}
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
		tinystr[0] = (char)(keynum + 61);
		tinystr[1] = 0;
		return tinystr;
	}

	if (keynum >= SDL_SCANCODE_1 && keynum <= SDL_SCANCODE_9) {
		tinystr[0] = (char)(keynum + 18);
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
	tinystr[2] = (char)(i > 9 ? i - 10 + 'a' : i + '0');
	tinystr[3] = (char)(j > 9 ? j - 10 + 'a' : j + '0');
	tinystr[4] = 0;

	return tinystr;
}