#pragma once

#include <nanovg.h>
#include <SDL/SDL.h>
#include "entityx.hh"

#include "components.h"
#include "sweep.h"

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

const Sweep Move(EntityManager * es, Entity ent, double dx, double dy, Entity &hitEnt);