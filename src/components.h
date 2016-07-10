#pragma once
struct Body {
	Body(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {}
	double x, y;
	double w, h;
};

struct Movable {
	Movable(double dx, double dy) : dx(dx), dy(dy) {}
	double dx, dy;
};

struct Renderable {
	Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};