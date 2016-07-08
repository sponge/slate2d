#pragma once

#include <nanovg.h>
#include <SDL/SDL.h>
#include "entityx.hh"

struct ClientInfo {
	SDL_Window *window;
	NVGcontext *nvg;
	int width, height;
};

struct Body {
	Body(double x, double y, int w, int h) : x(x), y(y), w(w), h(h) {}
	double x, y;
	int w, h;
};

struct Movable {
	Movable(double dx, double dy) : dx(dx), dy(dy) {}
	double dx, dy;
};

struct Renderable {
	Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
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