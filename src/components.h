#pragma once
#include "sweep.h"
#include "tmx.h"
#include "local.h"

struct Body : public Box {
	explicit Body(double x, double y, double w, double h) : Box(x, y, w, h) {	}
};

struct Movable {
	explicit Movable(double dx, double dy) : dx(dx), dy(dy) {}
	double dx, dy;
};

struct Renderable {
	explicit Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};

struct TileMap {
	explicit TileMap() {}
	tmx_map *map;
	tmx_layer *worldLayer;
};

struct Camera {
	explicit Camera(double x, double y, double w, double h) : size(w, h) {}
	bool active = true;
	Vec2 pos, size;
	double top, right, bottom, left;
	Body *target = nullptr;
};

void Cam_Move(Camera &cam, double x, double y);

struct PlayerInput {
	explicit PlayerInput() {}

	bool up = false, down = false, left = false, right = false, jump = false, attack = false, menu = false;
};