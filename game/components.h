#pragma once
#include "sweep.h"
#include <tmx.h>
#include "../src/image.h"

// when adding a new component:
// - update the enum
// - update GENERATE_COMPONENT calls in baseworld.h
// - update struct->lua binding in baseworld.cpp
// - update table generation in lua_pecs_system.cpp
// - update components.lua

enum {
	COMPONENT_ANY = 0,
	COMPONENT_BODY = 1 << 0,
	COMPONENT_MOVABLE = 1 << 1,
	COMPONENT_RENDERABLE = 1 << 2,
	COMPONENT_TILEMAP = 1 << 3,
	COMPONENT_CAMERA = 1 << 4,
	COMPONENT_PLAYERINPUT = 1 << 5,
	COMPONENT_PLAYER = 1 << 6,
	COMPONENT_SPRITE = 1 << 7
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

// not a component: used in tilemap
typedef struct TileInfo {
	TileInfo() {}
	int gid;
	bool solid;
	bool platform;
} tileinfo_t;

struct TileMap {
	TileMap() {}
	tmx_map *map;
	tmx_layer *worldLayer;
	TileInfo *tinfo;
};

struct Camera {
	Camera() {}
	explicit Camera(float w, float h, float scale = 0, float mx = 0, float my = 0) : size(w, h), max(mx, my), scale(scale) {}
	bool active = false;
	Vec2 pos, size, max;
	float scale;
	float top, right, bottom, left;
	int target = -1;

	void Center(float cx, float cy) {
		Move(cx - (size.x / 2) / scale, cy - (size.y / 2) / scale);
	};

	void Move(float x, float y) {
		pos.x = x;
		pos.y = y;
		top = y;
		right = x + (size.x / scale);
		bottom = y + (size.y / scale);
		left = x;
	};

	void Bind() {
		float x = pos.x, y = pos.y;

		if (left < 0) {
			x = 0;
		}

		if (right > max.x) {
			x = max.x - (size.x / scale);
		}

		if (top < 0) {
			y = 0;
		}

		if (bottom > max.y) {
			y = max.y - (size.y / scale);
		}

		Move(x, y);
	};
};

struct PlayerInput {
	explicit PlayerInput() {}

	bool up = false, down = false, left = false, right = false, jump = false, attack = false, menu = false;
};

struct Player {
	Player() {}
	uint8_t numJumps;
	bool isWallSliding;
	bool canWallJump;
	bool jumpHeld;
	bool willPogo;
	float stunTime;
};

struct Sprite {
	Sprite() {}
	explicit Sprite(unsigned int img, Vec2 size, Vec2 ofs) : img(img), ofs(ofs), size(size) {}
	explicit Sprite(unsigned int img, int sx, int sy, int ofsx, int ofsy) : img(img), ofs(ofsx, ofsy), size(sx, sy) {}
	unsigned int img;
	unsigned int frame;
	Vec2 ofs, size;
	bool flipX, flipY;
};