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

const Sweep Trace(ex::EntityManager &es, ex::Entity ent, float dx, float dy, ex::Entity &hitEnt);
Sweep Map_SweepTiles(TileMap &map, Box check, Vec2 delta, Vec2 tileSize);

extern bool consoleActive;

// main.cpp
#include "main.h"

// files.cpp
#include "files.h"

// input.cpp
#include "input.h"

// entities.cpp
#include "entities.h"