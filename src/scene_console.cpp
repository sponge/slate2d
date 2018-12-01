#include "scene_console.h"
#include <imgui.h>
#include "imgui_impl_sdl.h"
#include "cvar_main.h"
#include "console/console.h"

void ConsoleScene::Startup(ClientInfo* info) {
	inf = info;
}

void ConsoleScene::Update(double) {
	if (!consoleActive) {
		return;
	}

}

void ConsoleScene::Render() {
	if (vid_showfps->integer || com_pause->integer) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
		ImGui::SetNextWindowPos(ImVec2(inf->width - 80, 0));
		ImGui::SetNextWindowSize(ImVec2(80, 0));
		ImGui::Begin("##fps", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		if (vid_showfps->integer) {
			ImGui::Text("%.0f FPS\n%.3f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		}

		if (com_pause->integer) {
			ImGui::TextColored({ 255, 255, 0, 255 }, "Paused");
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	if (!consoleActive) {
		return;
	}

	IMConsole()->Draw("Console", &consoleActive);
}
