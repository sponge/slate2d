#pragma once

#include <nanovg.h>
#include <SDL/SDL.h>
#include "entityx.hh"

#include "components.h"
#include "sweep.h"
#include "console/console.h"

struct ClientInfo {
	SDL_Window *window;
	NVGcontext *nvg;
	int width, height;
};

typedef struct Img {
	NVGcontext *nvg;
	int hnd;
	int w, h;
	char path[64];
} img_t;

struct System {
	virtual ~System() {}
	virtual void update(EntityManager &es, double dt) = 0;
};

struct RenderSystem {
	virtual ~RenderSystem() {}
	virtual void render(EntityManager &es, ClientInfo *inf) = 0;
};

const Sweep Move(EntityManager * es, Entity ent, double dx, double dy, Entity &hitEnt);

// files.cpp

void FS_Init(const char *argv0);
int FS_ReadFile(const char *path, void **buffer);

// input.cpp

bool KeyEvent(int key, bool down, unsigned time);