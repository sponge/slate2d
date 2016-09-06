#pragma once
#include "sweep.h"
#include "tmx.h"
#include "local.h"

struct Body : public Box {
	Body(double x, double y, double w, double h) : Box(x, y, w, h) {	}
};

struct Movable {
	Movable(double dx, double dy) : dx(dx), dy(dy) {}
	double dx, dy;
};

struct Renderable {
	Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};

struct TileMap {
	TileMap() {}
	tmx_map *map;
	tmx_layer *worldLayer;
};

struct Camera {
	Camera(double x, double y, double w, double h) : size(w, h) {
		//inf = info;
		Move(x, y);
	}

	void Move(double x, double y) {
		pos.x = x;
		pos.y = y;
		top = y - size.y / 2;
		right = x + size.x / 2;
		bottom = y + size.y / 2;
		left = x - size.x / 2;
	}

	bool active = true;
	Vec2 pos, size;
	double top, right, bottom, left;
	Body *target = nullptr;
	//Entity target;
	//ClientInfo *inf;
};

struct PlayerInput {
	PlayerInput() {}

	bool up = false, down = false, left = false, right = false, jump = false, run = false, attack = false, menu = false;
};

using Components = entityx::Components<Body, Movable, Renderable, TileMap, Camera, PlayerInput>;
using EntityManager = entityx::EntityX<Components>;
template <typename C>
using Component = EntityManager::Component<C>;
using Entity = EntityManager::Entity;