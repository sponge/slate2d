#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#define NO_SDL_GLEXT
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

extern "C" {
#include "rlgl.h"
	extern bool initGL(int width, int height);
}

#include <imgui.h>
#include "imgui_impl_sdl.h"

#include "files.h"
#include "input.h"
#include "cvar_main.h"

#include "gamedll.h"

#include "scene_console.h"
#include "imgui_console.h"

#include "shared.h"

#include <soloud.h>
#include <soloud_thread.h>

#include "filewatcher.h"
#include "crunch_frontend.h"
#include "assetloader.h"

extern "C" {
#include "console.h"
}

conState_t console;

SoLoud::Soloud soloud;
ClientInfo inf;
int64_t last_update_musec = 0, frame_musec = 0, com_frameTime = 0;
//float frame_accum;
bool frameAdvance = false;
bool errorVisible = false;

gameExportFuncs_t * gexports;
SDL_Window *window;
ConsoleScene *consoleScene;

const char * __cdecl tempstr(const char *format, ...) {
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

void SetWindowTitle(const char *title) {
	SDL_SetWindowTitle(window, title);
}

void Cmd_FrameAdvance_f(void) {
	if (!eng_pause->integer) {
		Con_SetVar("engine.pause", "1");
	}
	else {
		frameAdvance = true;
	}
}

void Cmd_ToggleConsole_f(void) {
	if (consoleScene == nullptr) {
		return;
	}
	consoleScene->consoleActive = !consoleScene->consoleActive;
}

void Cmd_Vid_Restart_f(void) {
	inf.width = vid_width->integer;
	inf.height = vid_height->integer;

	SDL_SetWindowSize(window, inf.width, inf.height);
	SDL_GL_SetSwapInterval(vid_swapinterval->integer);
	SDL_SetWindowFullscreen(window, vid_fullscreen->integer == 2 ? SDL_WINDOW_FULLSCREEN : vid_fullscreen->integer == 1 ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Cmd_Exec_f() {
	if (Con_GetArgsCount() != 2) {
		Con_Printf("exec <filename> - runs the contents of filename as a console script\n");
		return;
	}

	sds name = sdsnew(Con_GetArg(1));

	if (strcmp(&name[sdslen(name) - 4], ".cfg") == 0) {

	}
	if (!FS_Exists(name)) {
		Con_Printf("couldn't exec file %s\n", name);
		return;
	}

	void *buffer;
	auto sz = FS_ReadFile(name, &buffer);
	const char *str = (const char *)buffer;

	Con_Execute(str);

	free(buffer);
	sdsfree(name);
}

void Cmd_Clear_f() {
	IMConsole()->ClearLog();
}

void DropToMenu() {
	errorVisible = true;
	gexports->Error(ERR_GAME, eng_errorMessage->string);
}

void ConH_Print(const char *line) {
	IMConsole()->AddLog("%s", line);
	printf(line);
}

void ConH_Error(int level, const char *message) {
	Con_Print(message);

#if defined(_WIN32) && defined(DEBUG)
	if (level == ERR_FATAL) {
		__debugbreak();
	}
#else
	if (level == ERR_FATAL) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", message, NULL);
	}
#endif

	if (level == ERR_FATAL) {
		exit(1);
	}
	else {
		Con_SetVar("engine.errorMessage", message);
		DropToMenu();
	}
}

static bool loop = true;

static void Cmd_Quit_f(void) {
	loop = false;
}

auto start = std::chrono::steady_clock::now();

static inline long long measure_now() {
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();
}

void main_loop() {

	auto now = measure_now();
	frame_musec = now - com_frameTime;
	com_frameTime = now;

	if (snd_volume->modified) {
		soloud.setGlobalVolume(snd_volume->value);
		snd_volume->modified = false;
	}

	FileWatcher_Tick();

	SDL_Event ev;
	ImGuiIO &io = ImGui::GetIO();
	while (SDL_PollEvent(&ev)) {
		ImGui_ImplSdl_ProcessEvent(&ev);

		switch (ev.type) {
		case SDL_QUIT:
			loop = false;
			return;
		case SDL_KEYDOWN:
			if (ev.key.keysym.sym == SDLK_BACKQUOTE) {
				consoleScene->consoleActive = !consoleScene->consoleActive;
				ImGui::SetWindowFocus(nullptr);
				break;
			}
		default:
			ProcessInputEvent(ev);
		}
	}
	
	//Cbuf_Execute(); // FIXME: the events would add keypresses to a buffer and then execute them all at once

	ImGui_ImplSdl_NewFrame(window);

	if (errorVisible && eng_errorMessage->string[0] == '\0') {
		errorVisible = 0;
	}
	else if (errorVisible) {
		ImGui::SetNextWindowPosCenter();
		ImGui::Begin("Error", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("%s", eng_errorMessage->string);
		ImGui::Text("%s", eng_lastErrorStack->string);
		ImGui::NewLine();
		if (ImGui::Button("Close")) {
			errorVisible = false;
			Con_SetVar("engine.errorMessage", nullptr);
			Con_SetVar("engine.lastErrorStack", nullptr);
		}
		ImGui::End();
	}

	rlMatrixMode(RL_PROJECTION);                            // Enable internal projection matrix
	rlLoadIdentity();                                       // Reset internal projection matrix
	rlOrtho(0.0, inf.width, inf.height, 0.0, 0.0, 1.0); // Recalculate internal projection matrix
	rlMatrixMode(RL_MODELVIEW);                             // Enable internal modelview matrix
	rlLoadIdentity();                                       // Reset internal modelview matrix

	bool ranFrame = gexports->Frame(!eng_pause->integer || frameAdvance ? frame_musec / 1E6 : 0);
	if (ranFrame) {
		last_update_musec = com_frameTime;
	}

	consoleScene->Update(frame_musec / 1E6);

	if (!eng_pause->integer || frameAdvance) {
		frameAdvance = false;
	}

	consoleScene->Render();

	ImGui::Render();
	ImGui_ImplSdl_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);

	// OSes seem to not be able to sleep for shorter than a millisecond. so let's sleep until
	// we're close-ish and then burn loop the rest. we get a majority of the cpu/power gains
	// while still remaining pretty accurate on frametimes.
	if (vid_maxfps->integer > 0) {
		long long target = now + (long long) (1000.0f / vid_maxfps->integer * 1000);
		long long currentSleepTime = measure_now();
		while (currentSleepTime <= target) {
			long long amt = (target - currentSleepTime) - 2000;
			if (amt > 0) {
				std::this_thread::sleep_for(std::chrono::microseconds(amt));
			}
			currentSleepTime = measure_now();
		}
	}

}

int main(int argc, char *argv[]) {
	// initialize console. construct imgui console, setup handlers, and then initialize the actual console
	IMConsole();
	console.handlers.print = &ConH_Print;
	console.handlers.getKeyForString = &Key_StringToKeynum;
	console.handlers.getStringForKey = &Key_KeynumToString;
	console.handlers.error = &ConH_Error;

	Con_Init(&console);
	Con_AllocateKeys(MAX_KEYS);

	// setup console to pull cvars from command line
	Con_SetupCommandLine(argc, argv);

	// we don't have a filesystem yet so we don't want to run the whole command line
	// yet. pick out the convars that are important for FS initialization, and then later on
	// we'll run the rest.
	Con_SetVarFromStartup("fs.basepath");
	Con_SetVarFromStartup("fs.basegame");
	Con_SetVarFromStartup("fs.game");
	FS_Init(argv[0]);

	// add engine level commands here
	Con_AddCommand("exec", Cmd_Exec_f);
	Con_AddCommand("quit", Cmd_Quit_f);
	Con_AddCommand("vid_restart", Cmd_Vid_Restart_f);
	Con_AddCommand("toggleconsole", Cmd_ToggleConsole_f);
	Con_AddCommand("frame_advance", Cmd_FrameAdvance_f);
	Con_AddCommand("clear", Cmd_Clear_f);

	RegisterMainCvars();
	FileWatcher_Init();
	Crunch_Init();

	if (!FS_Exists("default.cfg")) {
		Con_Error(ERR_FATAL, "Filesystem error, check fs_basepath is set correctly. (Could not find default.cfg)");
	}

	Con_Execute("exec default.cfg\n");
	if (FS_Exists("autoexec.cfg")) {
		Con_Execute("exec autoexec.cfg\n");
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		Con_Error(ERR_FATAL, "There was an error initing SDL2: %s", SDL_GetError());
	}

	atexit(SDL_Quit);
#ifdef __EMSCRIPTEN__
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

	inf.width = vid_width->integer;
	inf.height = vid_height->integer;
	window = SDL_CreateWindow("Slate2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, inf.width, inf.height, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		Con_Error(ERR_FATAL, "There was an error creating the window: %s", SDL_GetError());
	}

	SDL_SetWindowFullscreen(window, vid_fullscreen->integer == 2 ? SDL_WINDOW_FULLSCREEN : vid_fullscreen->integer == 1 ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!initGL(inf.width, inf.height)) {
		Con_Error(ERR_FATAL, "Could not init GL.");
	}

	SDL_GL_SetSwapInterval(vid_swapinterval->integer);

	if (context == NULL) {
		Con_Error(ERR_FATAL, "There was an error creating OpenGL context: %s", SDL_GetError());
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL) {
		Con_Error(ERR_FATAL, "There was an error with OpenGL configuration.");
	}

	soloud.init();
	// FIXME: check result code?

	SDL_GL_MakeCurrent(window, context);

	ImGui::CreateContext();
	ImGui_ImplSdl_Init(window);

	ImGui::StyleColorsDark();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);

	ImGuiIO &io = ImGui::GetIO();
#ifdef RELEASE
	io.IniFilename = NULL;
#endif

	// not working in emscripten for some reason? assert on ImGuiKey_Space not being mapped
#ifndef __EMSCRIPTEN__	
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#endif

	// now that we've ran the user configs and initialized everything else, apply everything else on the
	// command line here. this will set the rest of the variables and run any commands specified.
	Con_ExecuteCommandLine();

	consoleScene = new ConsoleScene();
	consoleScene->Startup(&inf);

#ifdef _WIN32
	static const char *lib = "game.dll";
#elif defined MACOS
	static const char *lib = "libgame.dylib";
#else
	static const char *lib = "libgame.so";
#endif

	Sys_LoadDll(lib, (void **)(&gexports));
	gexports->Init((void*)&inf, (void*)ImGui::GetCurrentContext());
	console.handlers.unhandledCommand = gexports->Console;

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (loop) {
		main_loop();
	}
#endif

	Con_Shutdown();
	Asset_ClearAll();
	ImGui_ImplSdl_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(context);

	return 0;
}
