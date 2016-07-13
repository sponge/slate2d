#pragma once
#include "sweep.h"
struct Body : Vec2 {
	Body(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {}
	double x, y;
	double w, h;
	Vec2 min() {
		return Vec2(x - w / 2, y - h / 2);
	}
	Vec2 max() {
		return Vec2(x + w / 2, y + h / 2);
	}
};

struct Movable {
	Movable(double dx, double dy) : dx(dx), dy(dy) {}
	double dx, dy;
};

struct Renderable {
	Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};