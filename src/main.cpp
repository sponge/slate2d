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

ClientInfo i;

int main(int argc, char *argv[]) {
#ifdef DEBUG
	testCollision();
#endif

	Cbuf_Init();
	Cmd_Init();
	Cvar_Init();

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "There was an error initing SDL2: " << SDL_GetError() << std::endl;
		return 1;
	}

	atexit(SDL_Quit);

	auto r_width = Cvar_Get("r_width", "1280", 0);
	auto r_height = Cvar_Get("r_height", "720", 0);

#ifndef _WIN32
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

	i.width = r_width->integer;
	i.height = r_height->integer;
	i.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, i.width, i.height, SDL_WINDOW_OPENGL);

	if (i.window == NULL) {
		std::cerr << "There was an error creating the window: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(i.window);

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

	SDL_GL_MakeCurrent(i.window, context);

	struct NVGcontext* vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
	i.nvg = vg;

	auto sm = new SceneManager(i);
	Scene* main_scene = new MapScene();
	sm->Switch(main_scene);
	auto perf_scene = new PerfScene();
	sm->Push(perf_scene);

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
					sm->Switch(newScene);
					sm->Push(perf_scene);
					delete(main_scene);
					main_scene = newScene;
				break;
				case SDLK_2:
					newScene = new TestScene();
					sm->Switch(newScene);
					sm->Push(perf_scene);
					delete(main_scene);
					main_scene = newScene;
				break;
				case SDLK_3:
					newScene = new MapScene();
					sm->Switch(newScene);
					sm->Push(perf_scene);
					delete(main_scene);
					main_scene = newScene;
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

		nvgBeginFrame(i.nvg, i.width, i.height, (float)i.width / i.height);
		sm->Render();
		nvgEndFrame(vg);

		SDL_GL_SwapWindow(i.window);
	}

	SDL_GL_DeleteContext(context);

	return 0;
}