#include <iostream>
#include <cmath>

#ifdef _WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <nanovg.h>
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#define AABB_IMPLEMENTATION
#include "sweep.h"

#include "perf/perf.h"
#include "local.h"

#include "scene.h"
#include "scene_main.h"
#include "scene_test.h"
#include "scene_map.h"
#include "scene_perf.h"
#include "scene_console.h"

ClientInfo inf;

int main(int argc, char *argv[]) {
#ifdef DEBUG
	testCollision();
#endif

	int len, i;
	for (len = 1, i = 1; i < argc; i++) {
		len += strlen(argv[i]) + 1;
	}

	char *cmdline = (char *) malloc(len);
	*cmdline = 0;
	for (i = 1; i < argc; i++)
	{
		if (i > 1) {
			strcat(cmdline, " ");
		}
		strcat(cmdline, argv[i]);
	}

	Com_ParseCommandLine(cmdline);

	Cbuf_Init();
	Cmd_Init();
	Cvar_Init();

	Com_AddStartupCommands();
	Cbuf_Execute();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "There was an error initing SDL2: " << SDL_GetError() << std::endl;
		return 1;
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
		std::cerr << "There was an error creating the window: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(inf.window);

	SDL_GL_SetSwapInterval(0);

	if (context == NULL) {
		std::cerr << "There was an error creating OpenGL context: " << SDL_GetError() << std::endl;
		return 1;
	}

	const unsigned char *version = glGetString(GL_VERSION);
	if (version == NULL) {
		std::cerr << "There was an error with OpenGL configuration:" << std::endl;
		return 1;
	}

#ifdef _WIN32
	if (glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
#endif

	SDL_GL_MakeCurrent(inf.window, context);

	struct NVGcontext* vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	inf.nvg = vg;

	auto sm = new SceneManager(inf);
	Scene* mainScene = new MapScene();
	sm->Switch(mainScene);
	auto perf_scene = new PerfScene();
	sm->Push(perf_scene);
	auto console_scene = new ConsoleScene();
	sm->Push(console_scene);

	bool quit = false;
	SDL_Event ev;

	int prevt = SDL_GetTicks();

	while (!quit) {
		int t, dt;

		t = SDL_GetTicks();
		dt = t - prevt;
		prevt = t;

		Cbuf_Execute();

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_KEYUP) {
				Scene* newScene;
				switch (ev.key.keysym.sym) {
				case SDLK_1:
					newScene = new MainScene();
					sm->Replace(mainScene, newScene);
					delete(mainScene);
					mainScene = newScene;
				break;
				case SDLK_2:
					newScene = new TestScene();
					sm->Replace(mainScene, newScene);
					delete(mainScene);
					mainScene = newScene;
				break;
				case SDLK_3:
					newScene = new MapScene();
					sm->Replace(mainScene, newScene);
					delete(mainScene);
					mainScene = newScene;
				break;
				}
			}
			else if (ev.type == SDL_QUIT) {
				quit = 1;
				break;
			}

			sm->Event(&ev);
		}

		sm->Update(dt / 1000.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		nvgBeginFrame(inf.nvg, inf.width, inf.height, (float)inf.width / inf.height);
		sm->Render();
		nvgEndFrame(vg);

		SDL_GL_SwapWindow(inf.window);
	}

	SDL_GL_DeleteContext(context);

	return 0;
}