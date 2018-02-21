#include <imgui.h>
#include "public.h"
#include "draw.h"

gameImportFuncs_t *trap;
kbutton_t in_1_left, in_1_right, in_1_up, in_1_run, in_1_down, in_1_jump, in_1_attack, in_1_menu;

void Com_DefaultExtension(char *path, int maxSize, const char *extension);

// map (name) - load a map and switch to the game scene
void Cmd_Map_f(void) {
	auto mapname = trap->Cmd_Argv(1);
	char filename[256];

	if (trap->Cmd_Argc() != 2) {
		trap->Print("map <mapname> : load a map\n");
		return;
	}

	snprintf(filename, sizeof(filename), "maps/%s", mapname);
	Com_DefaultExtension(filename, sizeof(filename), ".tmx");

	if (!trap->FS_Exists(filename)) {
		trap->Print("Map does not exist: %s\n", filename);
		return;
	}

	/*
	auto newScene = new GameMapScene(filename);
	
	trap->Scene_Replace(0, newScene);
	*/
}

AssetHandle dog, music, speech, font;

static void Init(void *clientInfo, void *imGuiContext) {
	trap->Cmd_AddCommand("map", Cmd_Map_f);

	trap->Cmd_AddCommand("+p1up",     []() { trap->IN_KeyDown(&in_1_up); });
	trap->Cmd_AddCommand("-p1up",     []() { trap->IN_KeyUp(&in_1_up); });
	trap->Cmd_AddCommand("+p1down",   []() { trap->IN_KeyDown(&in_1_down); });
	trap->Cmd_AddCommand("-p1down",   []() { trap->IN_KeyUp(&in_1_down); });
	trap->Cmd_AddCommand("+p1left",   []() { trap->IN_KeyDown(&in_1_left); });
	trap->Cmd_AddCommand("-p1left",   []() { trap->IN_KeyUp(&in_1_left); });
	trap->Cmd_AddCommand("+p1right",  []() { trap->IN_KeyDown(&in_1_right); });
	trap->Cmd_AddCommand("-p1right",  []() { trap->IN_KeyUp(&in_1_right); });
	trap->Cmd_AddCommand("+p1run",    []() { trap->IN_KeyDown(&in_1_run); });
	trap->Cmd_AddCommand("-p1run",    []() { trap->IN_KeyUp(&in_1_run); });
	trap->Cmd_AddCommand("+p1jump",   []() { trap->IN_KeyDown(&in_1_jump); });
	trap->Cmd_AddCommand("-p1jump",   []() { trap->IN_KeyUp(&in_1_jump); });
	trap->Cmd_AddCommand("+p1attack", []() { trap->IN_KeyDown(&in_1_attack); });
	trap->Cmd_AddCommand("-p1attack", []() { trap->IN_KeyUp(&in_1_attack); });
	trap->Cmd_AddCommand("+p1menu",   []() { trap->IN_KeyDown(&in_1_menu); });
	trap->Cmd_AddCommand("-p1menu",   []() { trap->IN_KeyUp(&in_1_menu); });

	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);

	//trap->Scene_Switch(new MenuScene());

	dog = trap->Asset_Create(ASSET_IMAGE, "dog", "gfx/dog.png");
	music = trap->Asset_Create(ASSET_MOD, "music", "music/frantic_-_dog_doesnt_care.it");
	speech = trap->Asset_Create(ASSET_SPEECH, "speech", "great job! you are a good dog!");
	font = trap->Asset_Create(ASSET_BITMAPFONT, "font", "gfx/good_neighbors.png");
	trap->BMPFNT_Set(font, "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", -1, 7, 16);

	trap->Asset_LoadAll();

	trap->Snd_Play(music, 1.0f, 0.0f, true);
	trap->Snd_Play(speech, 1.0f, 0.0f, false);
}

static void Console(const char *line) {

}

// technically the scene manager will handle every frame for gameplay scenes,
// but anything that needs an event loop type pump can go here
static void Frame(float dt) {
	DC_Clear();
	DC_SetColor(255, 0, 0, 255);
	DC_DrawRect(0, 0, 16, 16);
	DC_DrawImage(120, 120, 154, 16, 0, 0, 4.0, 0, dog, 0);
	DC_DrawBmpText(32, 50, 3.0f, "Good Dog!", font);
	DC_Submit();
}

static gameExportFuncs_t GAMEfuncs = {
	Init,
	Console,
	Frame
};

extern "C" 
#ifdef _WIN32
__declspec(dllexport)
#endif
void dllEntry(void ** exports, void * imports, int * version) {
	*exports = &GAMEfuncs;
	trap = (gameImportFuncs_t *)imports;
	*version = 1;
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
const char	* __cdecl va(const char *format, ...) {
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char	*buf;

	buf = string[index & 1];
	index++;

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

	return buf;
}

/*
==================
Com_DefaultExtension
==================
*/
void Com_DefaultExtension(char *path, int maxSize, const char *extension) {
	char	oldPath[1024];
	char    *src;

	//
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path) {
		if (*src == '.') {
			return;                 // it has an extension
		}
		src--;
	}

	strncpy(oldPath, path, sizeof(oldPath));
	snprintf(path, maxSize, "%s%s", oldPath, extension);
}