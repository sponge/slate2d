#pragma once
#include "../src/sweep.h"

enum {
	COMPONENT_ANY = 0,
	COMPONENT_BODY = 1 << 0,
	COMPONENT_MOVABLE = 1 << 1,
	COMPONENT_RENDERABLE = 1 << 2
};

struct Body : public Box {
	Body() : Box(0, 0, 0, 0) {}
	explicit Body(float x, float y, float w, float h) : Box(x, y, w, h) {	}
};

struct Movable {
	Movable() {}
	explicit Movable(float dx, float dy) : dx(dx), dy(dy) {}
	float dx, dy;
	bool upTouch, leftTouch, rightTouch, downTouch;
};

struct Renderable {
	Renderable() {}
	explicit Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};