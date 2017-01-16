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
	bool topTouch, leftTouch, rightTouch, downTouch;
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
	explicit Camera(double w, double h, double scale = 0, double mx = 0, double my = 0) : size(w, h), max(mx, my), scale(scale) {}
	bool active = true;
	Vec2 pos, size, max;
	double scale;
	double top, right, bottom, left;
	Body *target = nullptr;
};

void Cam_Move(Camera &cam, double x, double y);
void Cam_Center(Camera &cam, double cx, double cy);
void Cam_Bind(Camera &cam);

struct PlayerInput {
	explicit PlayerInput() {}

	bool up = false, down = false, left = false, right = false, jump = false, attack = false, menu = false;
};