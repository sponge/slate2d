#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include "files.h"
#include "input.h"
#include "cvar_main.h"

#include "gamedll.h"

#include "scene.h"
#include "scene_console.h"

#include "../game/public.h"

#include <soloud.h>
#include <soloud_thread.h>

SoLoud::Soloud soloud;
ClientInfo inf;
SceneManager *sm;
double frame_msec, com_frameTime;
tmx_map *map;
//float frame_accum;

gameExportFuncs_t * gexports;
SDL_Window *window;

void Cmd_Vid_Restart_f(void) {
	inf.width = vid_width->integer;
	inf.height = vid_height->integer;

	SDL_SetWindowSize(window, inf.width, inf.height);
	SDL_GL_SetSwapInterval(vid_swapinterval->integer);
	SDL_SetWindowFullscreen(window, vid_fullscreen->integer == 2 ? SDL_WINDOW_FULLSCREEN : vid_fullscreen->integer == 1 ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void DropToMenu() {
	sm->Clear();
}

int main(int argc, char *argv[]) {
	using clock = std::chrono::high_resolution_clock;
	using namespace std::chrono_literals;

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
	Cvar_Init();
	RegisterMainCvars();
	CL_InitKeyCommands();

	if (!FS_Exists("default.cfg")) {
		Com_Error(ERR_FATAL, "Filesystem error, check fs_basepath is set correctly. (Could not find default.cfg)");
	}

	Cbuf_AddText("exec default.cfg\n");
	if (FS_Exists("autoexec.cfg")) {
		Cbuf_AddText("exec autoexec.cfg\n");
	}
	Cbuf_Execute();

	Com_StartupVariable(nullptr);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Com_Error(ERR_FATAL, "There was an error initing SDL2: %s", SDL_GetError());
	}

	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

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

	if (glewInit() != GLEW_OK) {
		Com_Error(ERR_FATAL, "Could not init glew.");
	}

	soloud.init();

	SDL_GL_MakeCurrent(window, context);

	ImGui_ImplSdlGL3_Init(window);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);

	struct NVGcontext* vg = nvgCreateGL3(NVG_STENCIL_STROKES);
	inf.nvg = vg;

	unsigned char *font;
	auto sz = FS_ReadFile("/fonts/Roboto-Regular.ttf", (void **)&font);
	assert(sz != -1);
	nvgCreateFontMem(vg, "sans", font, sz, 1);

	if (!Com_AddStartupCommands()) {
		// do something here? don't load menu?
	}

	sm = new SceneManager(inf);

	auto consoleScene = new ConsoleScene();
	consoleScene->Startup(&inf);

	int ver = 0;

#ifdef _WIN32
	static const char *lib = "game.dll";
#elif defined MACOS
	static const char *lib = "libgame.dylib";
#endif

	Sys_LoadDll(lib, (void **)(&gexports), &ver);
	gexports->Init((void*)&inf, (void*)ImGui::GetCurrentContext());

	bool quit = false;
	SDL_Event ev;

	auto start = clock::now();
	auto last = clock::now();

	while (!quit) {
		auto now = clock::now();
		auto dt = std::chrono::duration<float>(now - last).count();
		com_frameTime = std::chrono::duration<float>(now - start).count() * 1000;
		frame_msec = dt * 1000;
		last = now;

		while (SDL_PollEvent(&ev)) {
			ImGui_ImplSdlGL3_ProcessEvent(&ev);

			if (ev.type == SDL_QUIT) {
				quit = 1;
				break;
			}

			if (ev.type == SDL_KEYUP) {
				KeyEvent(ev.key.keysym.scancode, false, com_frameTime);
			}

			if (ev.type == SDL_KEYDOWN) {
				KeyEvent(ev.key.keysym.scancode, true, com_frameTime);
			}

			if (ev.type == SDL_MOUSEBUTTONUP) {
				MouseEvent(ev.button.button, false, com_frameTime);
			}

			if (ev.type == SDL_MOUSEBUTTONDOWN) {
				MouseEvent(ev.button.button, true, com_frameTime);
			}
		}
		
		Cbuf_Execute();

		ImGui_ImplSdlGL3_NewFrame(window);

		/*
		frame_accum += dt;
		while (frame_accum >= 1 / 200.0f) {
			sm->Update(1 / 200.0f);
			frame_accum -= 1 / 200.0f;
		}
		*/

		glClear(GL_COLOR_BUFFER_BIT);
		nvgBeginFrame(inf.nvg, inf.width, inf.height, 1.0);

		gexports->Frame(dt);
		sm->Update(dt);
		consoleScene->Update(dt);

		sm->Render();
		consoleScene->Render();

		nvgEndFrame(vg);
		ImGui::Render();

		SDL_GL_SwapWindow(window);

		// sleep a little so we don't burn up cpu/gpu on insanely fast frames (>1000fps)
		if (frame_msec < 1.0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	SDL_GL_DeleteContext(context);

	return 0;
}