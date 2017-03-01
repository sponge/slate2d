#pragma once
#include "sweep.h"
#include <tmx.h>
#include "image.h"

struct Body : public Box {
	explicit Body(float x, float y, float w, float h) : Box(x, y, w, h) {	}
};

struct Trigger {
	float w;
	float h;
};

struct Movable {
	explicit Movable(float dx, float dy) : dx(dx), dy(dy) {}
	float dx, dy;
	bool upTouch, leftTouch, rightTouch, downTouch;
};

struct Renderable {
	explicit Renderable(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r, g, b, a;
};

typedef struct TileInfo {
	int gid;
	bool solid;
	bool platform;
} tileinfo_t;

struct TileMap {
	explicit TileMap() {}
	tmx_map *map;
	tmx_layer *worldLayer;
	TileInfo *tinfo;
};

const Sweep Trace(entityx::EntityManager &es, entityx::Entity ent, float dx, float dy, entityx::Entity &hitEnt);
Sweep Map_SweepTiles(TileMap &map, Box check, Vec2 delta, Vec2 tileSize);

struct Camera {
	explicit Camera(float w, float h, float scale = 0, float mx = 0, float my = 0) : size(w, h), max(mx, my), scale(scale) {}
	bool active = true;
	Vec2 pos, size, max;
	float scale;
	float top, right, bottom, left;
	Body *target = nullptr;
};

void Cam_Move(Camera &cam, float x, float y);
void Cam_Center(Camera &cam, float cx, float cy);
void Cam_Bind(Camera &cam);

struct PlayerInput {
	explicit PlayerInput() {}

	bool up = false, down = false, left = false, right = false, jump = false, attack = false, menu = false;
};

struct Player {
	uint8_t numJumps;
	bool isWallSliding;
	bool canWallJump;
	bool jumpHeld;
	bool willPogo;
	float stunTime;
};

struct Sprite {
	explicit Sprite(Img *img, Vec2 size, Vec2 ofs) : img(img), ofs(ofs), size(size) {}
	Img *img;
	unsigned int frame;
	Vec2 ofs, size;
	bool flipX, flipY;
};