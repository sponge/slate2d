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

#include <nanovg.h>
#ifdef __EMSCRIPTEN__
#define NANOVG_GLES2_IMPLEMENTATION
#else
#define NANOVG_GL3_IMPLEMENTATION
#endif
#include <nanovg_gl.h>

#include <imgui.h>
#include "imgui_impl_sdl.h"

#include "files.h"
#include "input.h"
#include "cvar_main.h"

#include "gamedll.h"

#include "scene.h"
#include "scene_console.h"

#include "../game/public.h"

#include <soloud.h>
#include <soloud_thread.h>

#include "filetracker.h"

SoLoud::Soloud soloud;
ClientInfo inf;
SceneManager *sm;
int64_t frame_musec = 0, com_frameTime = 0;
tmx_map *map;
//float frame_accum;
bool frameAdvance = false;
bool errorVisible = false;

gameExportFuncs_t * gexports;
SDL_Window *window;
ConsoleScene *consoleScene;

void Cmd_FrameAdvance_f(void) {
	if (!com_pause->integer) {
		Cvar_Set("com_pause", "1");
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

void DropToMenu() {
	sm->Clear();
	errorVisible = true;
}


static struct NVGcontext* vg;
static bool loop = true;

using namespace std::chrono;
using namespace std::chrono_literals;

auto start = steady_clock::now();

void main_loop() {

	auto now = duration_cast<microseconds>( steady_clock::now() - start ).count();
	frame_musec = now - com_frameTime;
	com_frameTime = now;

	if (s_volume->modified) {
		soloud.setGlobalVolume(s_volume->value);
		s_volume->modified = false;
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

		case SDL_KEYUP:
			KeyEvent(ev.key.keysym.scancode, false, com_frameTime);
			break;

		case SDL_KEYDOWN:
			if (ev.key.keysym.sym == SDLK_BACKQUOTE) {
				consoleScene->consoleActive = !consoleScene->consoleActive;
				ImGui::SetWindowFocus(nullptr);
				break;
			}
			if (io.WantCaptureKeyboard) {
				break;
			}
			KeyEvent(ev.key.keysym.scancode, true, com_frameTime);
			break;

		case SDL_CONTROLLERDEVICEADDED: {
			if (ev.cdevice.which > MAX_CONTROLLERS) {
				break;
			}

			SDL_GameController *controller = SDL_GameControllerOpen(ev.cdevice.which);
			Com_Printf("Using controller at device index %i: %s\n", ev.cdevice.which, SDL_GameControllerName(controller));
			break;
		}

		case SDL_CONTROLLERDEVICEREMOVED: {
			SDL_GameController* controller = SDL_GameControllerFromInstanceID(ev.cdevice.which);
			Com_Printf("Closing controller instance %i: %s\n", ev.cdevice.which, SDL_GameControllerName(controller));
			SDL_GameControllerClose(controller);
			break;
		}

		case SDL_MOUSEBUTTONUP:
			MouseEvent(ev.button.button, false, com_frameTime);
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
				break;
			}
			MouseEvent(ev.button.button, true, com_frameTime);
			break;


		case SDL_CONTROLLERBUTTONDOWN:
			JoyEvent(ev.jbutton.which, ev.jbutton.button, true, com_frameTime);
			break;

		case SDL_CONTROLLERBUTTONUP:
			JoyEvent(ev.jbutton.which, ev.jbutton.button, false, com_frameTime);
			break;
		}
	}
	
	Cbuf_Execute();

	ImGui_ImplSdl_NewFrame(window);

	if (errorVisible && strlen(com_errorMessage->string) == 0) {
		errorVisible = 0;
	}
	else if (errorVisible) {
		ImGui::SetNextWindowPosCenter();
		ImGui::Begin("Error", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("%s", com_errorMessage->string);
		ImGui::Text("%s", com_lastErrorStack->string);
		ImGui::NewLine();
		if (ImGui::Button("Close")) {
			errorVisible = false;
			Cvar_Set("com_errorMessage", nullptr);
			Cvar_Set("com_lastErrorStack", nullptr);
		}
		ImGui::End();
	}

	glClear(GL_COLOR_BUFFER_BIT);
	nvgBeginFrame(inf.nvg, inf.width, inf.height, 1.0);

	gexports->Frame(frame_musec / 1E6);
	consoleScene->Update(frame_musec / 1E6);

	if (!com_pause->integer || frameAdvance) {
		sm->Update(frame_musec / 1E6);
		frameAdvance = false;
	}

	sm->Render();
	consoleScene->Render();

	nvgEndFrame(vg);
	ImGui::Render();
	ImGui_ImplSdl_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(window);

	// sleep a little so we don't burn up cpu/gpu on insanely fast frames (>1000fps)
	// in a perfect world you should be able to do com_maxFps but we lose the cpu benefits
	// by having to burn through a really tight loop to measure
	if (com_sleepShortFrame->integer && frame_musec < 1000.0f) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

}

int main(int argc, char *argv[]) {
	// handle command line parsing. combine into one string and pass it in.
	if (argc > 1) {
		int len, i;
		for (len = 1, i = 1; i < argc; i++) {
			len += strlen(argv[i]) + 1;
		}

		char *cmdline = (char *)malloc(len);
		*cmdline = 0;
		for (i = 1; i < argc; i++)
		{
			if (i > 1) {
				strcat(cmdline, " ");
			}
			strcat(cmdline, argv[i]);
		}
		Com_ParseCommandLine(cmdline);
	}

	Com_StartupVariable("fs_basepath");
	Com_StartupVariable("fs_basegame");
	Com_StartupVariable("fs_game");
	FS_Init(argv[0]);

	Cbuf_Init();
	Cmd_Init();
	Cmd_AddCommand("vid_restart", Cmd_Vid_Restart_f);
	Cmd_AddCommand("toggleconsole", Cmd_ToggleConsole_f);
	Cmd_AddCommand("frame_advance", Cmd_FrameAdvance_f);
	Cvar_Init();
	RegisterMainCvars();
	CL_InitKeyCommands();
	FileWatcher_Init();

	if (!FS_Exists("default.cfg")) {
		Com_Error(ERR_FATAL, "Filesystem error, check fs_basepath is set correctly. (Could not find default.cfg)");
	}

	Cbuf_AddText("exec default.cfg\n");
	if (FS_Exists("autoexec.cfg")) {
		Cbuf_AddText("exec autoexec.cfg\n");
	}
	Cbuf_Execute();

	Com_StartupVariable(nullptr);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		Com_Error(ERR_FATAL, "There was an error initing SDL2: %s", SDL_GetError());
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
	window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, inf.width, inf.height, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		Com_Error(ERR_FATAL, "There was an error creating the window: %s", SDL_GetError());
	}

	SDL_SetWindowFullscreen(window, vid_fullscreen->integer == 2 ? SDL_WINDOW_FULLSCREEN : vid_fullscreen->integer == 1 ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(vid_swapinterval->integer);

	if (context == NULL) {
		Com_Error(ERR_FATAL, "There was an error creating OpenGL context: %s", SDL_GetError());
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL) {
		Com_Error(ERR_FATAL, "There was an error with OpenGL configuration.");
	}

#ifndef __EMSCRIPTEN__
	if (glewInit() != GLEW_OK) {
		Com_Error(ERR_FATAL, "Could not init glew.");
	}
#endif

	soloud.init();

	SDL_GL_MakeCurrent(window, context);

	ImGui::CreateContext();
	ImGui_ImplSdl_Init(window);

	ImGui::StyleColorsDark();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);
#ifdef __EMSCRIPTEN__
	vg = nvgCreateGLES2(NVG_STENCIL_STROKES);
#else
	vg = nvgCreateGL3(NVG_STENCIL_STROKES);
#endif
	inf.nvg = vg;

	unsigned char *font;
	auto sz = FS_ReadFile("/fonts/Roboto-Regular.ttf", (void **)&font);
	assert(sz != -1);
	nvgCreateFontMem(vg, "sans", font, sz, 1);

	if (!Com_AddStartupCommands()) {
		// do something here? don't load menu?
	}

	sm = new SceneManager(inf);

	consoleScene = new ConsoleScene();
	consoleScene->Startup(&inf);

	int ver = 0;

#ifdef _WIN32
	static const char *lib = "game.dll";
#elif defined MACOS
static const char *lib = "libgame.dylib";
#else
        static const char *lib = "libgame.so";
#endif

	Sys_LoadDll(lib, (void **)(&gexports), &ver);
	gexports->Init((void*)&inf, (void*)ImGui::GetCurrentContext());

// not working in emscripten for some reason? assert on ImGuiKey_Space not being mapped
#ifndef __EMSCRIPTEN__	
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#endif

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (loop) {
		main_loop();
	}
#endif

	ImGui_ImplSdl_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(context);

	return 0;
}
