#pragma once

#include <nanovg.h>
#include <SDL/SDL.h>

#include <entityx/entityx.h>
namespace ex = entityx;

#include "components.h"

#include "sweep.h"
#include "console/console.h"

#ifdef _MSC_VER 
#define STRFUNCS
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

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

const Sweep Trace(ex::EntityManager &es, ex::Entity ent, float dx, float dy, ex::Entity &hitEnt);
Sweep Map_SweepTiles(TileMap &map, Box check, Vec2 delta, Vec2 tileSize);

extern bool consoleActive;

// main.cpp

extern int com_frameTime;
extern int frame_msec;

void DropToMenu();

// files.cpp

void FS_Init(const char *argv0);
int FS_ReadFile(const char *path, void **buffer);
bool FS_Exists(const char *file);
char** FS_List(const char *path);
void FS_FreeList(void * listVar);

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

// entities.cpp
void SpawnEntitiesFromLayer(tmx_layer &layer, ex::EntityManager &es);
bool SpawnEntity(const char *type, ex::EntityManager &es, tmx_object &obj);