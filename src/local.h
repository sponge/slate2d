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

extern int com_frameTime;
extern int frame_msec;
extern bool consoleActive;

// files.cpp

void FS_Init(const char *argv0);
int FS_ReadFile(const char *path, void **buffer);

// input.cpp

typedef struct {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	bool		active;			// current state
	bool		wasPressed;		// set when down, not cleared when up
} kbutton_t;

extern kbutton_t in_1_left, in_1_right, in_1_up, in_1_down, in_1_jump, in_1_attack, in_1_menu;

bool KeyEvent(int key, bool down, unsigned time);
void IN_KeyDown(kbutton_t *b);
void IN_KeyUp(kbutton_t *b);
void CL_InitInput(void);
float CL_KeyState(kbutton_t *key);