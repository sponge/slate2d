#include "scene_console.h"
#include <nanovg.h>
#include <random>
#include <tmx.h>

void ConsoleScene::Startup(ClientInfo* info) {
	inf = info;
}

void ConsoleScene::Update(double dt) {

}

void ConsoleScene::Render() {
	if (!consoleActive) {
		return;
	}

	auto nvg = inf->nvg;

	nvgBeginPath(nvg);
	nvgFillColor(nvg, nvgRGBA(50, 50, 50, 200));
	nvgRect(nvg, 0, 0, inf->width, inf->height / 2);
	nvgFill(nvg);

	nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255));
	nvgTextAlign(nvg, NVG_ALIGN_LEFT);
	nvgText(nvg, 10, inf->height / 2 - 12, currentLine, 0);
}

void ConsoleScene::Teardown() {

}

bool ConsoleScene::Event(SDL_Event *ev) {
	switch (ev->type) {
	case SDL_KEYDOWN:
		switch (ev->key.keysym.sym) {
		case SDLK_BACKQUOTE:
			consoleActive = !consoleActive;
			break;

		case SDLK_RETURN:
		case SDLK_RETURN2:
			Cbuf_ExecuteText(EXEC_NOW, currentLine);
			currentLine[0] = '\0';
			break;

		case SDLK_BACKSPACE:
			int len = strlen(currentLine);
			if (len > 0) {
				currentLine[len - 1] = '\0';
			}
			break;
		}


		break;

	case SDL_TEXTINPUT:
		if (ev->text.text[0] == '`') {
			break;
		}
		strncat(currentLine, ev->text.text, MAX_CMD_LINE);
		break;
	}

	return !consoleActive;
}