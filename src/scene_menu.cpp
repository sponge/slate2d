#include "scene_menu.h"
#include "console/console.h"
#include "cvar_main.h"

void MenuScene::Startup(ClientInfo* info) {
	inf = info;
	rawMaps = FS_List("maps/");
	char **map;

	for (map = rawMaps; *map != NULL && mapSize < MAX_MAPS; map++) {
		if (strstr(*map, ".tmx") == nullptr) {
			continue;
		}
		maps[mapSize] = *map;
		mapSize++;
	}

	if (strlen(com_errorMessage->string) > 0) {
		showError = true;
	}
}

MenuScene::~MenuScene() {
	FS_FreeList(rawMaps);
	if (strlen(com_errorMessage->string) > 0) {
		Cvar_Set("com_errorMessage", "");
	}
}

void MenuScene::Update(float dt) {

}

void MenuScene::Render() {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.01, 0.14, 0.45, 0.4));

		ImGui::SetNextWindowSize(ImVec2(300, 300));
		ImGui::SetNextWindowPosCenter();
		ImGui::Begin("Main Menu", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
			
		ImGui::PushItemWidth(128);
		ImGui::Combo("##input", &selected, maps, mapSize);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Load Map")) {
			auto str = va("map %s\n", maps[selected]);
			Cbuf_ExecuteText(EXEC_NOW, str);
		}

		if (ImGui::Button("Test Scene 1")) {
			Cbuf_ExecuteText(EXEC_NOW, "scene 1\n");
		}

		if (ImGui::Button("Test Scene 2")) {
			Cbuf_ExecuteText(EXEC_NOW, "scene 2\n");		
		}
		
		ImGui::End();

		if (showError) {
			ImGui::OpenPopup("Error");
			showError = false;
		}

		if (ImGui::BeginPopupModal("Error")) {
			ImGui::Text(com_errorMessage->string);
			if (ImGui::Button("OK")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	ImGui::PopStyleColor();
}