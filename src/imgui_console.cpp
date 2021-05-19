#include <imgui.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "console.h"
#include "imgui_console.h"
#include "cvar_main.h"
#include "main.h"

#define CONSOLE_MAX_LINES 4000

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

ConsoleUI::ConsoleUI() {
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;
}

ConsoleUI::~ConsoleUI() {
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}

void ConsoleUI::ClearLog() {
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
	ScrollToBottom = true;
}

void ConsoleUI::AddLog(const char* fmt, ...) {
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);

  std::stringstream ss(buf);
  std::string to;

	while (std::getline(ss,to,'\n')) {
		Items.push_back(Strdup(to.c_str()));
	}
	
	ScrollToBottom = true;

	if (Items.size() > CONSOLE_MAX_LINES) {
		free(Items[0]);
		Items.erase(Items.begin());
	}
}

void ConsoleUI::Draw(int width, int height) {

	if (vid_showfps->integer || eng_pause->integer) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
		ImGui::SetNextWindowPos(ImVec2((float)width - 80, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(80, 0));
		ImGui::Begin("##fps", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		if (vid_showfps->integer) {
			ImGui::Text("%.0f FPS\n%.3f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		}

		if (eng_pause->integer) {
			ImGui::TextColored({ 255, 255, 0, 255 }, "Paused");
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	if (consoleActive == false) {
		return;
	}

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Slate2D")) {
			if (ImGui::MenuItem("Quit")) {
				Con_Execute("quit");
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug Windows")) {
			if (ImGui::MenuItem("Asset Inspector", nullptr, debug_assets->boolean)) {
				Con_SetVarFloat("debug.assets", debug_assets->integer ? 0 : 1);
			}

			if (ImGui::MenuItem("Font Atlas", nullptr, debug_fontAtlas->boolean)) {
				Con_SetVarFloat("debug.fontAtlas", debug_fontAtlas->integer ? 0 : 1);
			}

			if (ImGui::MenuItem("ImGui Demo", nullptr, debug_imguidemo->boolean)) {
				Con_SetVarFloat("debug.imguiDemo", debug_imguidemo->integer ? 0 : 1);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console", &consoleActive))
	{
		ImGui::End();
		return;
	}

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::Selectable("Clear")) ClearLog();
		ImGui::EndPopup();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	ImGuiListClipper clipper(Items.Size);
	while (clipper.Step()) {
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			const char* item = Items[i];
			ImGui::TextUnformatted(item);
		}
	}

	if (ScrollToBottom || (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
	ScrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();

	// Command-line
	ImGui::PushItemWidth(-1);
	if (ImGui::InputText("", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
	{
		char* input_end = InputBuf + strlen(InputBuf);
		while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
		if (InputBuf[0])
			ExecCommand(InputBuf);
		strcpy(InputBuf, "");
	}
	ImGui::PopItemWidth();

	// Demonstrate keeping auto focus on the input box
	if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

	ImGui::End();
}

void ConsoleUI::ExecCommand(const char* command_line) {
	AddLog("# %s\n", command_line);

	// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
	HistoryPos = -1;
	for (int i = History.Size - 1; i >= 0; i--)
		if (Stricmp(History[i], command_line) == 0)
		{
			free(History[i]);
			History.erase(History.begin() + i);
			break;
		}
	History.push_back(Strdup(command_line));

	Con_Execute(command_line);
}

int ConsoleUI::TextEditCallbackStub(ImGuiInputTextCallbackData* data) { // In C++11 you are better off using lambdas for this sort of forwarding callbacks
	ConsoleUI* console = (ConsoleUI*)data->UserData;
	return console->TextEditCallback(data);
}

int ConsoleUI::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		// Example of TEXT COMPLETION

		// Locate beginning of current word
		const char* word_end = data->Buf + data->CursorPos;
		const char* word_start = word_end;
		while (word_start > data->Buf)
		{
			const char c = word_start[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';')
				break;
			word_start--;
		}

		if (word_start[0] == '\0') {
			break;
		}

		candidates.clear();

		conState_t *con = Con_GetActive();

		map_iter_t iter = map_iter(&con->cmds);
		const char *key;
		while ((key = map_next(&con->cmds, &iter))) {
			if (strncasecmp(word_start, key, strlen(word_start)) == 0) {
				IMConsole()->candidates.push_back(key);
			}
		}

		iter = map_iter(&con->vars);
		key = nullptr;
		while ((key = map_next(&con->vars, &iter))) {
			if (strncasecmp(word_start, key, strlen(word_start)) == 0) {
				IMConsole()->candidates.push_back(key);
			}
		}

		if (candidates.Size == 0)
		{
			// No match
			AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
		}
		else if (candidates.Size == 1)
		{
			// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
			data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
			data->InsertChars(data->CursorPos, candidates[0]);
			data->InsertChars(data->CursorPos, " ");
		}
		else
		{
			// Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
			int match_len = (int)(word_end - word_start);
			for (;;)
			{
				int c = 0;
				bool all_candidates_matches = true;
				for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
					if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
				if (!all_candidates_matches)
					break;
				match_len++;
			}

			if (match_len > 0)
			{
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
			}

			// List matches
			AddLog("Possible matches:\n");
			for (int i = 0; i < candidates.Size; i++)
				AddLog("- %s\n", candidates[i]);
		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		// Example of HISTORY
		const int prev_history_pos = HistoryPos;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (HistoryPos == -1)
				HistoryPos = History.Size - 1;
			else if (HistoryPos > 0)
				HistoryPos--;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (HistoryPos != -1)
				if (++HistoryPos >= History.Size)
					HistoryPos = -1;
		}

		// A better implementation would preserve the data on the current input line along with cursor position.
		if (prev_history_pos != HistoryPos)
		{
			data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
			data->BufDirty = true;
		}
	}
	}
	return 0;
}

ConsoleUI * IMConsole() {
	static ConsoleUI console;
	return &console;
}