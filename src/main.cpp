#include <iostream>
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#include <imgui.h>
#include "imgui_impl_sdl_gl3.h"

#include <physfs.h>

#define AABB_IMPLEMENTATION
#include "sweep.h"

#include "local.h"

#include "scene.h"
#include "scene_menu.h"
#include "scene_test.h"
#include "scene_testbounce.h"
//#include "scene_map.h"
#include "scene_console.h"

ClientInfo inf;
SceneManager *sm;
Scene* mainScene;
int frame_msec, com_frameTime;

void Cmd_Scene_f(void) {
	auto num = atoi(Cmd_Argv(1));
	if (num < 0 || num > 3) {
		Com_Printf("invalid scene, specify 0, 1, 2, or 3\n");
		return;
	}
	Scene* newScene;
	switch (num) {
	case 0: default: newScene = new MenuScene(); break;
	case 1: newScene = new TestBounceScene(); break;
	case 2: newScene = new TestScene(); break;
	//case 3: newScene = new MapScene(); break;
	}

	sm->Replace(mainScene, newScene);
	delete(mainScene);
	mainScene = newScene;
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
	testCollision();
#endif

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
		free(cmdline);
	}

	Com_StartupVariable("fs_basepath");
	FS_Init(argv[0]);

	Cbuf_Init();
	Cmd_Init();
	Cmd_AddCommand("scene", Cmd_Scene_f);
	Cvar_Init();
	CL_InitKeyCommands();
	CL_InitInput();

	Com_AddStartupCommands();

	if (!PHYSFS_exists("default.cfg")) {
		Com_Error(ERR_FATAL, "Filesystem error, check fs_basepath is set correctly.\n");
	}

	Cbuf_AddText("exec default.cfg\n");
	if (PHYSFS_exists("autoexec.cfg")) {
		Cbuf_AddText("exec autoexec.cfg\n");
	}
	Cbuf_Execute();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		Com_Error(ERR_FATAL, "There was an error initing SDL2: %s\n", SDL_GetError());
	}

	atexit(SDL_Quit);

	auto vid_width = Cvar_Get("vid_width", "1280", 0);
	auto vid_height = Cvar_Get("vid_height", "720", 0);

#ifndef _WIN32
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

	inf.width = vid_width->integer;
	inf.height = vid_height->integer;
	inf.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, inf.width, inf.height, SDL_WINDOW_OPENGL);

	if (inf.window == NULL) {
		Com_Error(ERR_FATAL, "There was an error creating the window: %s\n", SDL_GetError());
	}

	SDL_GLContext context = SDL_GL_CreateContext(inf.window);

	SDL_GL_SetSwapInterval(0);

	if (context == NULL) {
		Com_Error(ERR_FATAL, "There was an error creating OpenGL context: %s\n", SDL_GetError());
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL) {
		Com_Error(ERR_FATAL, "There was an error with OpenGL configuration.\n");
	}

	if (glewInit() != GLEW_OK) {
		Com_Error(ERR_FATAL, "Could not init glew.\n");
	}

	SDL_GL_MakeCurrent(inf.window, context);

	ImGui_ImplSdlGL3_Init(inf.window);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0);

	struct NVGcontext* vg = nvgCreateGL3(NVG_STENCIL_STROKES);
	inf.nvg = vg;

	unsigned char *font;
	auto sz = FS_ReadFile("/fonts/Roboto-Regular.ttf", (void **)&font);
	assert(sz != -1);
	nvgCreateFontMem(vg, "sans", font, sz, 1);

	sm = new SceneManager(inf);
	mainScene = new TestBounceScene();
	sm->Switch(mainScene);
	auto console_scene = new ConsoleScene();
	sm->Push(console_scene);

	bool quit = false;
	SDL_Event ev;

	int prevt = SDL_GetTicks();

	while (!quit) {
		com_frameTime = SDL_GetTicks();
		frame_msec = com_frameTime - prevt;
		prevt = com_frameTime;

		Cbuf_Execute();

		while (SDL_PollEvent(&ev)) {
			ImGui_ImplSdlGL3_ProcessEvent(&ev);

			if (ev.type == SDL_QUIT) {
				quit = 1;
				break;
			}

			if (ev.type == SDL_KEYUP) {
				KeyEvent(ev.key.keysym.scancode, false, com_frameTime);
			}

			auto propagate = sm->Event(&ev);
			if (!propagate) {
				continue;
			}

			if (ev.type == SDL_KEYDOWN) {
				KeyEvent(ev.key.keysym.scancode, true, com_frameTime);
			}
		}
		
		Cbuf_Execute();

		ImGui_ImplSdlGL3_NewFrame(inf.window);
		sm->Update(frame_msec / 1000.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		nvgBeginFrame(inf.nvg, inf.width, inf.height, (float)inf.width / inf.height);
		sm->Render();
		nvgEndFrame(vg);

		ImGui::Render();

		SDL_GL_SwapWindow(inf.window);
	}

	SDL_GL_DeleteContext(context);

	return 0;
}