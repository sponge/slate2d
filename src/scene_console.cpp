#include "scene_console.h"
#include <nanovg.h>
#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"
#include "cvar_main.h"

cvar_t *r_showfps;
bool consoleActive;

void ConsoleScene::Startup(ClientInfo* info) {
	inf = info;
#if 0
	SDL_RWops *io = SDL_RWFromFile("temp.txt", "rb");
	if (io != NULL) {
		io->read(io, history, sizeof(history), 1);
		io->close(io);
	}
#endif
}

void ConsoleScene::Update(float dt) {
	if (!consoleActive) {
		return;
	}

}

void ConsoleScene::Render() {
	if (vid_showfps->integer) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
		ImGui::SetNextWindowPos(ImVec2(inf->width - 80, 0));
		ImGui::SetNextWindowSize(ImVec2(80, 0));
		ImGui::Begin("", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
		ImGui::Text("%.0f FPS\n%.3f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	if (!consoleActive) {
		return;
	}

	Console()->Draw("Console", &consoleActive);
}

bool ConsoleScene::Event(SDL_Event *ev) {
	switch (ev->type) {
	case SDL_KEYDOWN:
		switch (ev->key.keysym.sym) {
		case SDLK_BACKQUOTE:
			consoleActive = !consoleActive;
			break;
		}
	}

	return !consoleActive;
}