#pragma once
#include "../src/sweep.h"

// when adding a new component:
// - update the enum
// - update baseworld GENERATE_COMPONENT calls
// - update lua binding
// - update components.lua

enum {
	COMPONENT_ANY = 0,
	COMPONENT_BODY = 1 << 0,
	COMPONENT_MOVABLE = 1 << 1,
	COMPONENT_RENDERABLE = 1 << 2
};

struct Body {
	Body() {}
	Body(double x, double y, double w, double h) : x(x), y(y), w(w), h(h), hw(w*0.5), hh(h*0.5) {}
	double x, y, w, h, hw, hh;
	Vec2 min() {
		return Vec2(x - hw, y - hh);
	};
	Vec2 max() {
		return Vec2(x + hh, y + hh);
	};
};

struct Movable {
	Movable() {}
	explicit Movable(double dx, double dy) : dx(dx), dy(dy) {}
	double dx, dy;
	bool upTouch, leftTouch, rightTouch, downTouch;
};

struct Renderable {
	Renderable() {}
	explicit Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};