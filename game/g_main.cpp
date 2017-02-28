#include <imgui.h>

#include "public.h"
#include "scene_menu.h"

gameImportFuncs_t *trap;

// FIXME: abstract out rendering so i can just pass a list of drawing commands instead of nvg context stuff
static void Init(void *clientInfo, void *imGuiContext) {
	ImGui::SetCurrentContext((ImGuiContext*)imGuiContext);
	trap->Scene_Switch(new MenuScene());
}

static gameExportFuncs_t GAMEfuncs = {
	Init,
};

// FIXME: windows only :(
extern "C" __declspec(dllexport) void dllEntry(void ** exports, void * imports, int * version) {
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