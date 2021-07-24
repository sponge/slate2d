#include <SDL/SDL.h>
#include <imgui.h>
#include <ctype.h>
#include "console.h"
#include "input.h"
#include "keys.h"
#include "main.h"
#include <cmath>
#include <cstdint>
#include "cvar_main.h"

extern "C" const char * keys[MAX_KEYS];

static inline void JoyEvent(int controller, int button, bool down, int64_t time) {
	if (controller >= MAX_CONTROLLERS) {
		Con_Printf("ignoring controller %i > MAX_CONTROLLERS\n", controller);
	}

	int keyId = CONTROLLER_KEY_START + (controller * SDL_CONTROLLER_BUTTON_MAX) + button;

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

		Con_HandleKeyPress(SDL_NUM_SCANCODES + ev.button.button, true, com_frameTime);
		break;

	case SDL_CONTROLLERBUTTONDOWN:
		JoyEvent(ev.jbutton.which, ev.jbutton.button, true, com_frameTime);
		break;

	case SDL_CONTROLLERBUTTONUP:
		JoyEvent(ev.jbutton.which, ev.jbutton.button, false, com_frameTime);
		break;
	}
}

bool In_ButtonPressed(int buttonId, unsigned int delay, int repeat) {
	const buttonState_t *button = Con_GetButton(buttonId);

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

	if (com_frameTime >= firstTrigger && last_update_musec < firstTrigger) {
		return true;
	}

	int64_t heldTime = com_frameTime - button->timestamp - muDelay;

	if (heldTime < 0) {
		return false;
	}

	if (muRepeat == 0) {
		return true;
	}

	if (muRepeat < 0) {
		return false;
	}

	int64_t repeatCount = heldTime / muRepeat;
	int64_t lastRepeatCount = (heldTime - frame_musec) / muRepeat;
	//Con_Printf("current:%0.5f last:%0.5f", heldTime / muRepeat, (heldTime - frame_msec) / muRepeat);
	//Com_Printf(lastRepeatCount != repeatCount ? " TRIGGER\n" : "\n");

	return lastRepeatCount != repeatCount;
}

MousePosition In_MousePosition() {
	MousePosition mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	return mousePos;
}

AnalogAxes In_ControllerAnalog(int controllerNum) {
	AnalogAxes axes;
	SDL_GameController *controller = SDL_GameControllerFromInstanceID(controllerNum);
	axes.leftX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32768.0f;
	axes.leftY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32768.0f;
	axes.rightX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32768.0f;
	axes.rightY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32768.0f;
	axes.triggerLeft = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32768.0f;
	axes.triggerRight = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32768.0f;

	axes.leftX = fabs(axes.leftX) < in_deadzone->value ? 0.0f : axes.leftX;
	axes.leftY = fabs(axes.leftY) < in_deadzone->value ? 0.0f : axes.leftY;
	axes.rightX = fabs(axes.rightX) < in_deadzone->value ? 0.0f : axes.rightX;
	axes.rightY = fabs(axes.rightY) < in_deadzone->value ? 0.0f : axes.rightY;

	return axes;
}

int In_GetKeyNum(const char *str) {
	for (int i = 0; i < MAX_KEYS; i++) {
		if (keys[i] == NULL) {
			continue;
		}

		if (strcasecmp(str, keys[i]) == 0) {
			return i;
		}
	}

	return 0;
}

const char * In_GetKeyName(int key) {
	if (key <= 0 || key >= MAX_KEYS) {
		return "<UNKNOWN";
	}

	return keys[key];
}