#pragma once

#include <nanovg.h>
#include <SDL/SDL.h>
#include "entityx.hh"

#include "components.h"

struct ClientInfo {
	SDL_Window *window;
	NVGcontext *nvg;
	int width, height;
};

using Components = entityx::Components<Body, Movable, Renderable>;
using EntityManager = entityx::EntityX<Components>;
template <typename C>
using Component = EntityManager::Component<C>;
using Entity = EntityManager::Entity;

struct System {
	virtual ~System() {}
	virtual void update(EntityManager &es, double dt) = 0;
};

struct RenderSystem {
	virtual ~RenderSystem() {}
	virtual void render(EntityManager &es, ClientInfo *inf) = 0;
};

typedef struct {
	double time, timeRemaining, x, y, normalX, normalY;
} trace_t;

bool AABBCheck(Body b1, Body b2);
bool AABB(Body b1, Body b2, double& moveX, double& moveY);
Body GetSweptBroadphaseBox(Body b, double dx, double dy);
double SweptAABB(Body b1, Body b2, double dx, double dy, double& normalx, double& normaly);
const trace_t Move(EntityManager *es, Entity ent, double dx, double dy);