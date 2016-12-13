#include "scene_menu.h"

void MenuScene::Startup(ClientInfo* info) {
	inf = info;
}

void MenuScene::Update(double dt) {

}

void MenuScene::Render() {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.01, 0.14, 0.45, 0.4));
	ImGui::SetNextWindowSize(ImVec2(inf->width, inf->height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
	ImGui::End();
	//ImGui::ShowTestWindow();
	ImGui::PopStyleColor();
}