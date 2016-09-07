#include "scene_console.h"
#include <nanovg.h>
#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"

cvar_t *r_showfps;

void ConsoleScene::Startup(ClientInfo* info) {
	inf = info;
	r_showfps = Cvar_Get("r_showfps", "1", 0);
#if 0
	SDL_RWops *io = SDL_RWFromFile("temp.txt", "rb");
	if (io != NULL) {
		io->read(io, history, sizeof(history), 1);
		io->close(io);
	}
#endif
}

void ConsoleScene::Update(double dt) {
	if (!consoleActive) {
		return;
	}

}

void ConsoleScene::Render() {
	
	if (r_showfps->integer) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
		ImGui::SetNextWindowPos(ImVec2(inf->width - 80, 0));
		ImGui::SetNextWindowSize(ImVec2(80, 0));
		ImGui::Begin("", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
		ImGui::Text("%.1f FPS\n%.3f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	if (!consoleActive) {
		return;
	}
	
	ImGui::ShowTestWindow();

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