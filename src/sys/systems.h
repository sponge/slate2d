#pragma once
#include "../local.h"

struct RectDrawSystem : RenderSystem {
	void render(EntityManager &es, ClientInfo *inf);
};

struct RectMoverSystem : System {
	void update(EntityManager &es, double dt);
};

struct TileMapDrawSystem : RenderSystem {
	void render(EntityManager &es, ClientInfo *inf);
	void draw_image(ClientInfo *inf, tmx_map * map, tmx_layer *layer);
	void draw_tiles(ClientInfo *inf, tmx_map * map, tmx_layer *layer);
};

struct CameraUpdateSystem : System {
	void update(EntityManager &es, double dt);
};

struct CameraDrawSystem : RenderSystem {
	void render(EntityManager &es, ClientInfo *inf);
};